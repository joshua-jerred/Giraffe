const genericResponse = require("../http_api/generic_response");

const Database = require("../db");

const SEQUENCE_STEPS = {
  prelaunch: {
    title: "Pre-Launch Sequence",
    steps: {
      initialize_sequencer: {
        title: "Initialize Sequencer",
        description: "Initialize the sequencer for the flight",
        user_input: {
          type: "button",
          label: "initialize",
          action: "initialize_sequencer",
        },
      },
      prelaunch_connect_to_gfs: {
        title: "Connect to GFS TCP",
        description: "Connect to the GFS TCP server",
      },
      set_launch_position: {
        title: "Set Launch Position",
        description: "Set the launch position for the flight",
      },
      gdl_connection: {
        title: "GDL Connected",
        description: "Ensure that the Data Link is connected",
      },
      start_mission_clock: {
        title: "Start Mission Clock",
        description: "Start the mission clock to track flight time",
      },
      pass_bit_test: {
        title: "Pass BIT Test",
        description: "Perform Built-In Test to ensure systems are operational",
      },
      enter_launch_phase: {
        title: "Enter Launch Phase",
        description: "Transition the flight system to the launch phase",
      },
    },
  },
  launch: {
    title: "Launch Sequence",
    steps: {
      detect_launch: {
        title: "Receive Launch Detection Notification",
        description: "GFS will announce the launch detection via GDL",
      },
    },
  },
  // ascent: {
  // title: "Ascent Sequence",
  // steps: {},
  // },
  // descent: {
  // title: "Descent Sequence",
  // steps: {},
  // },
  recovery: {
    title: "Recovery Sequence",
    steps: {
      recovery_connect_to_gfs: {
        title: "Connect to GFS via TCP",
        description: "Reconnect to the GFS TCP server for recovery operations",
      },
      disable_gdl: {
        title: "Disable GDL",
        description: "Disable the Data Link",
      },
      download_flight_data: {
        title: "Download Flight Data",
        description: "Retrieve all flight data from GFS",
      },
      reset_flight_phase_to_prelaunch: {
        title: "Reset Flight Phase to Pre-Launch",
        description: "Reset the flight phase for another flight",
      },
    },
  },
};

const STEP_INIT_STATUS = {
  completed: false,
  status: "not started",
};

module.exports = class Sequencer {
  constructor(global_state) {
    this.global_state = global_state;

    // Initialize the sequence with all phases and steps
    this.sequence = {};
    for (const phase in SEQUENCE_STEPS) {
      // this.sequence[phase] = {};
      for (const step in SEQUENCE_STEPS[phase].steps) {
        this.sequence[step] = {
          ...STEP_INIT_STATUS,
        };
      }
    }

    this.current_phase = "unknown";
    this.current_step = null;
    this.sequencer_initialized = false;
    this.sequencer_run_id = null;

    // Load the last completed step from the database
    Database.models.SequencerStep.findOne({
      order: [["id", "DESC"]],
    })
      .then((lastStep) => {
        if (!lastStep) {
          return;
        }

        if (lastStep.step_label === "reset_flight_phase_to_prelaunch") {
          return; // Do not reset the sequencer if the last step was a reset
        }

        // Set the current step and mark all previous steps as completed
        const allSteps = this.#getStepList();
        let loaded_current_step = this.#getNextStep(lastStep.step_label);
        if (loaded_current_step === null) {
          console.warn(
            `No next step found for last completed step: ${lastStep.step_label}.`
          );
          return;
        }

        for (const step of allSteps) {
          if (step === loaded_current_step) {
            break; // Stop at the current step
          }
          this.sequence[step].completed = true;
          this.sequence[step].status = "completed";
        }

        this.current_step = loaded_current_step;
        this.#setStepStatus(this.current_step, "waiting");
        this.sequencer_run_id = lastStep.sequencer_run_id;
        this.sequencer_initialized = true;
      })
      .catch((error) => {
        console.error("Error loading last completed step:", error);
      });
  }

  #initializeSequencer(onSuccess, onFailure) {
    if (this.sequencer_initialized) {
      console.warn("Sequencer is already initialized.");
      return;
    }

    // Generate a unique run ID for this sequence
    Database.models.SequencerStep.findOne({
      order: [["id", "DESC"]],
    })
      .then((lastStep) => {
        if (lastStep === null) {
          this.sequencer_run_id = 1; // Start from 1 if no previous steps exist
        } else {
          this.sequencer_run_id = lastStep.sequencer_run_id + 1;
        }

        this.sequencer_initialized = true;
        console.log(
          "Sequencer initialized with run ID:",
          this.sequencer_run_id
        );
        this.#completeStep("initialize_sequencer", "completed");
        onSuccess();
      })
      .catch((error) => {
        console.error("Error initializing sequencer:", error);
        this.sequencer_initialized = false;
        onFailure(error.message);
      });
  }

  getSequenceMetadata() {
    return SEQUENCE_STEPS;
  }

  getSequenceData() {
    return {
      current_phase: this.current_phase,
      current_step: this.current_step,
      sequence: this.sequence,
    };
  }

  handlePostRequest(req, res) {
    if (!req.body.hasOwnProperty("step_label")) {
      genericResponse(res, 400, "step_label is required.");
      return;
    }

    const stepLabel = req.body.step_label;
    this.#doSequenceStep(
      stepLabel,
      () => {
        genericResponse(res, 200, "Step executed successfully.");
      },
      (error) => {
        genericResponse(res, 500, `Failed to execute step: ${error}`);
      }
    );
  }

  #doSequenceStep(step, onSuccess, onFailure) {
    if (this.current_step !== step) {
      console.warn(
        `Cannot execute step ${step} because current step is ${this.current_step}.`
      );
      onFailure(`Current step is ${this.current_step}, not ${step}.`);
      return;
    }

    let success = false;
    if (step === "initialize_sequencer") {
      if (this.sequencer_initialized) {
        console.warn("Sequencer is already initialized.");
        onFailure("Sequencer is already initialized.");
        return;
      }

      this.#initializeSequencer(onSuccess, onFailure);
    }

    return success;
  }

  #completeStep(step, status = "completed") {
    if (!this.sequence.hasOwnProperty(step)) {
      console.warn(`Step ${step} does not exist in the sequence.`);
      return;
    }

    this.sequence[step].status = status;
    this.sequence[step].completed = true;

    // Report the step completion to the database
    Database.models.SequencerStep.create({
      sequencer_run_id: this.sequencer_run_id,
      step_label: step,
      status: status,
    })
      .then(() => {
        // no action
      })
      .catch((error) => {
        console.error("Error saving step completion to database:", error);
      });

    // Find the next step in the phase
    const phase = this.current_phase.toLowerCase();
    const steps = SEQUENCE_STEPS[phase]?.steps;
    if (!steps) {
      console.warn(`No steps found for phase ${phase}.`);
      return;
    }
    const stepKeys = Object.keys(steps);
    console.log(`Current phase: ${phase}, steps:`, stepKeys);
    const currentIndex = stepKeys.indexOf(step);
    if (currentIndex === -1 || currentIndex === stepKeys.length - 1) {
      console.warn(
        `Step ${step} is the last step in phase ${phase}. Have not implemented phase transition yet.`
      );
      this.current_step = null; // No next step
      return;
    }
    this.current_step = stepKeys[currentIndex + 1];
    this.#setStepStatus(this.current_step, "waiting");
  }

  #getNextStep(step) {
    // Search through all phases for the next step
    let allSteps = [];
    for (const phase in SEQUENCE_STEPS) {
      SEQUENCE_STEPS[phase].steps &&
        allSteps.push(...Object.keys(SEQUENCE_STEPS[phase].steps));
    }

    const stepIndex = allSteps.indexOf(step);
    if (stepIndex === -1 || stepIndex === allSteps.length - 1) {
      console.warn(`Step ${step} is the last step or does not exist.`);
      return null; // No next step
    }
    return allSteps[stepIndex + 1];
  }

  #getStepList() {
    let allSteps = [];
    for (const phase in SEQUENCE_STEPS) {
      SEQUENCE_STEPS[phase].steps &&
        allSteps.push(...Object.keys(SEQUENCE_STEPS[phase].steps));
    }
    return allSteps;
  }

  #setStepStatus(step, status) {
    if (this.sequence.hasOwnProperty(step)) {
      this.sequence[step].status = status;
    } else {
      console.warn(`Step ${step} does not exist in the sequence.`);
    }
  }

  cycle() {
    const generalData = this.global_state.flight_data.getData("general");
    this.current_phase = generalData.flight_phase
      .toLowerCase()
      .replace("-", "");

    if (!this.sequencer_initialized) {
      this.current_step = "initialize_sequencer";
      this.#setStepStatus(this.current_step, "waiting");
    }
  }

  #checkPreLaunchSequence() {
    console.log("Checking Pre-Launch Sequence");
  }
};
