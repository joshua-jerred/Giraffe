module.exports = (global_state) => {
  const express = require("express");
  const path = require("path");
  const cors = require("cors");

  const app = express();
  const server = require("http").createServer(app);

  const apiRoutes = require("./routes");
  const httpSchema = require("./http_schema");

  var request_counter = function (req, res, next) {
    global_state.ggs_status.total_http_requests++;
    next();
  };

  // Middleware
  app.use(cors({ origin: "*" }));
  app.use(express.json());
  app.use(request_counter);
  app.use(httpSchema.verifyPathSchema);

  // API Handlers
  app.use("/api", apiRoutes(global_state)); 
  app.use("/static", express.static(path.join(__dirname, "..public")));

  // 404 Handler
  app.use((req, res, next) => {
    res.status(404).send("Not Found");
  });
  
  return server;
};
