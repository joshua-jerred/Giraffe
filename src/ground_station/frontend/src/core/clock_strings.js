export function getCurrentUtc(include_label = false) {
  let date = new Date();
  return (
    date.toISOString().split("T")[1].split(".")[0] +
    (include_label ? " UTC" : "")
  );
}

export function unixTimeToShortUtc(unix_time) {
  let date = new Date(unix_time * 1000);
  return date.toISOString().split("T")[1].split(".")[0] + " UTC";
}

export function unixTimeToLongUtc(unix_time) {
  let date = new Date(unix_time * 1000);
  return date.toISOString().split(".")[0];
}

export function isoMilliseconds(iso_time) {
  let date = new Date(iso_time);
  return date.toISOString();
}

export function unixTimeToLocal(unix_time) {
  let date = new Date(unix_time * 1000);
  return date.toLocaleTimeString();
}

export function clockDisplay(seconds) {
  const data = {
    days: Math.floor(seconds / 86400),
    hours: Math.floor(seconds / 3600),
    minutes: Math.floor((seconds % 3600) / 60),
    seconds: Math.floor(seconds % 60),
  };

  let output = "";
  if (data.days > 0) {
    output += `${data.days}d `;
  }

  if (data.hours > 0) {
    output += `${data.hours}h `;
  }

  if (data.minutes > 0) {
    output += `${data.minutes}m `;
  }

  if (data.seconds < 10) {
    output += `0${data.seconds}s`;
  } else {
    output += `${data.seconds}s`;
  }

  return output;
}
