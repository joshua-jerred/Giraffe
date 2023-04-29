const express = require("express");
const path = require("path");
const app = express();
const cors = require("cors");
const PORT = 3020; //process.env.PORT || 3006;

const httpRoutes = require("./http_api/routes");
const httpSchema = require("./http_api/http_schema");

const GlobalState = require("./state");
const global_state = new GlobalState();

// For testing purposes
var request_counter = function (req, res, next) {
  global_state.total_http_requests++;
  next();
}

// HTTP Server Setup
app.use(cors({ origin: "*" }));
app.use(express.json());

// Middleware Setup
app.use(request_counter);
app.use(httpSchema.verifyPathSchema);

// API Handler Setup
app.use("/api", httpRoutes(global_state));

// API Endpoint - /api/static/*
app.use("/static", express.static(path.join(__dirname, "public")));

// 404 Handler
app.use((req, res, next) => {
  res.status(404).send("Not Found");
});

// Enable the server
app.listen(PORT, () => {
  //console.log(`app is running on PORT ${PORT}`);
});

module.exports = app;