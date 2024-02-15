module.exports = function (meta_data, key, value) {

  const valid_types = ["string", "number", "boolean"];

  if (
    !(meta_data.hasOwnProperty(key) &&
    meta_data[key].hasOwnProperty("type"))
  ) {
    return [false, "item not found in metadata"];
  } else {
    let actual_type = typeof value;
    if (!valid_types.includes(actual_type)) {
      return [false, "invalid type: " + actual_type];
    }

    return [true, ""];
  }
};
