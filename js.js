// Self-referential function
function self() {
  return self();
}

// Infinite loop
while (true) {
  console.log("Hello, world!");
}

// Recursion to the max
function factorial(n) {
  if (n === 1) {
    return 1;
  } else {
    return n * factorial(n - 1);
  }
}

// Regular expression to match any string
const anyStringRegex = /.*/;

// Event listener that fires on every event
document.addEventListener("*", () => {});

// Object with an infinite number of properties
const infiniteObject = {};
for (let i = 0; i < Infinity; i++) {
  infiniteObject[i] = i;
}

// Function that returns a different value every time it is called
function randomFunction() {
  return Math.random();
}
