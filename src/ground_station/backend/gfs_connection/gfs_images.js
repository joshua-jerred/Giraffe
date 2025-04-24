const fs = require("fs");
const net = require("net");

const file_paths = require("../file_paths.js");
const { RequestMessage } = require("giraffe-protocol");

module.exports = class GfsImages {
  constructor(global_state) {
    this.global_state = global_state;
    this.image_dir = file_paths.GGS_IMAGE_DIR;

    // schema for image_info:
    // { "img_name.jpg": { "local": true, "remote": false }}
    this.images_info = {};

    this.#updateLocalImageList();
  }

  getImagesInfo() {
    return this.images_info;
  }

  /**
   * @brief This function retrieves an image from GFS and saves it locally
   * @param {*} filename
   */
  retrieveRemoteImage(filename, callback) {
    if (
      !this.images_info.hasOwnProperty(filename) ||
      !this.images_info[filename].remote
    ) {
      this.global_state.error(
        "gfs_images.js, retrieveRemoteImage, image not found on remote: " +
          filename
      );
      callback(false, "Image not available on remote.");
      return;
    }

    let msg = new RequestMessage("ggs", "gfs", "img/" + filename);

    let con = new net.Socket();

    let received_data = Buffer.alloc(0);

    const TIMEOUT = 500;
    con.setTimeout(TIMEOUT);
    con.connect(
      this.global_state.gfs_connection.port,
      this.global_state.gfs_connection.address,
      function () {
        con.write(msg.getString());
      }
    );
    con.on("data", function (data) {
      received_data = Buffer.concat([received_data, data]);
    });
    con.on("close", () => {
      // console.log("Connection closed");
      try {
        if (this.#parseImageData(received_data, filename)) {
          callback(true, "Image saved locally");
        }
      } catch (error) {
        callback(false, "Error: " + error);
      }
    });
    con.on("timeout", function () {
      callback(false, "Timeout");
      con.destroy();
    });
    con.on("error", function (error) {
      callback(false, "Error: " + error);
      con.destroy();
    });
  }

  updateRemoteImageList(files) {
    for (const img of files) {
      if (!this.images_info.hasOwnProperty(img)) {
        this.images_info[img] = { local: false, remote: true };
      } else {
        this.images_info[img].remote = true;
      }
    }
  }

  #updateLocalImageList() {
    let local_images = fs.readdirSync(this.image_dir);
    for (const img of local_images) {
      if (!this.images_info.hasOwnProperty(img)) {
        this.images_info[img] = { local: true, remote: false };
      } else {
        this.images_info[img].local = true;
      }
    }
    // console.log("Local image list updated");
    // console.log(this.local_image_list);
  }

  #parseImageData(raw_data, filename) {
    let total_size = raw_data.length;
    if (total_size < 8) {
      this.global_state.error(
        `gfs_images.js, parseImageData failure[0]: total_size: ${total_size}`
      );
      throw new Error("Invalid image data - see log");
    }
    // first 4 bytes, and last 4 bytes are the length of the image
    let length = raw_data.readUInt32LE(0);
    if (length > total_size - 8) {
      this.global_state.error(
        `gfs_images.js, parseImageData failure[1]: length: ${length}, total_size: ${total_size}`
      );
      throw new Error("Invalid image data - see log");
    }

    let img_data = raw_data.slice(4, length + 4);
    let end_length = raw_data.readUInt32LE(length + 4);

    if (total_size !== length + 8 || end_length !== length || length === 0) {
      this.global_state.error(
        `gfs_images.js, parseImageData failure[2]: total_size: ${total_size}, length: ${length}, end_length: ${end_length}`
      );
      throw new Error("Invalid image data - see log");
    }

    fs.writeFileSync(this.image_dir + "/" + filename, img_data);
    this.#updateLocalImageList();
    return true;
  }
};
