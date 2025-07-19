const SEQUENCE_STEPS = {
  prelaunch: {
    title: "Pre-Launch Sequence",
    steps: {
      prelaunch_connected_to_gfs: {
        title: "Connected to GFS TCP",
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

module.exports = class Sequencer {
  constructor(global_state) {
    this.global_state = global_state;

    // Initialize the sequence with all phases and steps
    this.sequence = {};
    for (const phase in SEQUENCE_STEPS) {
      this.sequence[phase] = {};
      for (const step in SEQUENCE_STEPS[phase].steps) {
        this.sequence[phase][step] = {
          completed: false,
          status: "not started",
        };
      }
    }

    this.current_phase = "unknown";
  }

  getSequenceMetadata() {
    return SEQUENCE_STEPS;
  }

  getSequenceData() {
    return {
      current_phase: this.current_phase,
      sequence: this.sequence,
    };
  }

  cycle() {
    const generalData = this.global_state.flight_data.getData("general");
    this.current_phase = generalData.flight_phase.toLowerCase();
    // console.log(generalData);
  }

  checkPreLaunchSequence() {}
};
