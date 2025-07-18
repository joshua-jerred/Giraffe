module.exports = function (res, res_code, message) {
  res.status(res_code).json({ message: message });
};

