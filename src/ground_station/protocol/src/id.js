module.exports = () => {
  const size = 8;
  let out = [...Array(size)].map(() => Math.floor(Math.random() * 16).toString(16)).join('');
  return out;
};
