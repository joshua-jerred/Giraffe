/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   protocol.js
 * @brief  The base Giraffe Protocol implementation for javascript.
 * @addtogroup protocol
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-06-11
 * @copyright  2023 (license to be defined)
 */

const valid_endpoints = ["gfs", "ggs", "gdl", "gwc", "fsa"];
const valid_types = ["req", "set", "rsp"];

function generateId() {
  const size = 8;
  let out = [...Array(size)]
    .map(() => Math.floor(Math.random() * 16).toString(16))
    .join("");
  return out;
}

/**
 * @brief The base JS message class for the Giraffe Protocol.
 */
module.exports = class Message {
  constructor(src = null, dst = null, typ = null, id = null, bdy = {}) {
    this.src = src;
    this.dst = dst;
    this.typ = typ;
    this.id = id;
    this.bdy = bdy;

    // Verify source validity
    if (!valid_endpoints.includes(this.src))
      throw new Error("Invalid source: " + this.src);

    // Verify destination validity
    if (!valid_endpoints.includes(this.dst) && this.dst !== null) {
      throw new Error("Invalid destination: " + this.dst);
    }

    // Verify type validity
    if (!valid_types.includes(this.typ) && this.typ !== null) {
      throw new Error("Invalid type: " + this.typ);
    }

    // Verify id validity
    if (typeof this.id !== "string") {
      this.id = generateId();
    }

    // Verify body validity
    if (typeof this.bdy !== "object" && this.bdy !== null) {
      throw new Error("Invalid body: " + this.bdy);
    }
  }

  get json() {
    return this.getJson();
  }

  getJson() {
    return {
      src: this.src,
      dst: this.dst,
      typ: this.typ,
      id: this.id,
      bdy: this.bdy,
    };
  }

  getString() {
    return JSON.stringify(this.getJson());
  }

  toString() {
    return JSON.stringify(this.getJson());
  }

  getResponseCode() {
    if (
      this.bdy !== null &&
      this.bdy.cde !== undefined &&
      (this.bdy.cde === "er" || this.bdy.cde === "ok")
    ) {
      return this.bdy.cde;
    }
    return "un";
  }
};
