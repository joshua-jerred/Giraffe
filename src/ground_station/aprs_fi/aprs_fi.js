const superagent = require("superagent");

module.exports = async function aprs_fi() {
  // callback
  superagent
    .get("http://localhost:8080/")
    .set("Accept", "application/json")
    .set("User-Agent", "example-agent")
    // .query({ action: "edit", city: "London" }) // query string
    // .unset("User-Agent") // removes the User-Agent header
    // .send({ name: "Manny", species: "cat" }) // sends a JSON post body
    // .set("X-API-Key", "foobar")
    // .set("accept", "json")
    .end((err, res) => {
      console.log(res.body);
      // Calling the end function will send the request
    });

  // promise with then/catch
  // superagent.post("/").then(console.log).catch(console.error);

  // promise with async/await
  // (async () => {
  //   try {
  //     const res = await superagent.post("/api/pet");
  //     console.log(res);
  //   } catch (err) {
  //     console.error(err);
  //   }
  // })();
};
