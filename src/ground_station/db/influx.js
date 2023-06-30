//repl.repl.ignoreUndefined = true;

// const { InfluxDB, Point } = require("@influxdata/influxdb-client");

// const token = process.env.INFLUXDB_TOKEN;
// const url = "http://192.168.0.3:12023";

// const client = new InfluxDB({ url, token });

// let org = `giraffe`;
// let bucket = `giraffe_ground_station`;

// let writeClient = client.getWriteApi(org, bucket, "ns");

// for (let i = 0; i < 5; i++) {
//   let point = new Point("measurement1")
//     .tag("tagname1", "tagvalue1")
//     .intField("field1", i);

//   void setTimeout(() => {
//     writeClient.writePoint(point);
//   }, i * 1000); // separate points by 1 second

//   void setTimeout(() => {
//     writeClient.flush();
//   }, 5000);
// }

// Execute a Flux Query
// let queryClient = client.getQueryApi(org);
// let fluxQuery = `from(bucket: "giraffe_ground_station")
//  |> range(start: -10m)
//  |> filter(fn: (r) => r._measurement == "measurement1")`;

// queryClient.queryRows(fluxQuery, {
//   next: (row, tableMeta) => {
//     const tableObject = tableMeta.toObject(row);
//     console.log(tableObject);
//   },
//   error: (error) => {
//     console.error("\nError", error);
//   },
//   complete: () => {
//     console.log("\nSuccess");
//   },
// });

queryClient = client.getQueryApi(org);
fluxQuery = `from(bucket: "giraffe_ground_station")
 |> range(start: -10m)
 |> filter(fn: (r) => r._measurement == "measurement1")
 |> mean()`;

queryClient.queryRows(fluxQuery, {
  next: (row, tableMeta) => {
    const tableObject = tableMeta.toObject(row);
    console.log(tableObject);
  },
  error: (error) => {
    console.error("\nError", error);
  },
  complete: () => {
    console.log("\nSuccess");
  },
});
