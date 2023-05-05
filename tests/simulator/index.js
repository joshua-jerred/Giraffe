const config = require("./sim_params.json");
const Environment = require("./environment.js");
const GFS_Sim = require("./gfs_sim.js");

if (config.mode === "full") {
  console.log("Starting the Environment Simulator");
  const env = new Environment();
  env.begin(false);
  console.log("Starting the GFS Simulator");
  GFS_Sim(env);
} else {
  console.log("Not implemented yet");
}
