const canvas = document.getElementById("canvas");
const ctx = canvas.getContext("2d");
let originalImage = null;

// Load default image on startup
const img = new Image();
img.crossOrigin = "anonymous";
img.src = "https://upload.wikimedia.org/wikipedia/commons/thumb/3/36/Hopetoun_falls.jpg/640px-Hopetoun_falls.jpg";
img.onload = () => {
  ctx.drawImage(img, 0, 0, canvas.width, canvas.height);
  originalImage = ctx.getImageData(0, 0, canvas.width, canvas.height);
};

// Handle image upload
document.getElementById("upload").addEventListener("change", function(e) {
  const file = e.target.files[0];
  const reader = new FileReader();
  reader.onload = function(event) {
    const newImg = new Image();
    newImg.onload = function() {
      ctx.drawImage(newImg, 0, 0, canvas.width, canvas.height);
      originalImage = ctx.getImageData(0, 0, canvas.width, canvas.height);
    };
    newImg.src = event.target.result;
  };
  reader.readAsDataURL(file);
});

// Reset to original image
function reset() {
  if (originalImage) {
    ctx.putImageData(originalImage, 0, 0);
  }
}

// Apply blur or sharpen
function applyFilter(type) {
  const kernels = {
    blur: [
      [1 / 9, 1 / 9, 1 / 9],
      [1 / 9, 1 / 9, 1 / 9],
      [1 / 9, 1 / 9, 1 / 9],
    ],
    sharpen: [
      [ 0, -1,  0],
      [-1,  5, -1],
      [ 0, -1,  0],
    ]
  };

  const kernel = kernels[type];
  const imageData = ctx.getImageData(0, 0, canvas.width, canvas.height);
  const data = imageData.data;
  const copy = new Uint8ClampedArray(data); // Backup

  const width = canvas.width;
  const height = canvas.height;

  for (let y = 1; y < height - 1; y++) {
    for (let x = 1; x < width - 1; x++) {
      for (let c = 0; c < 3; c++) { // R, G, B channels
        let sum = 0;
        for (let ky = -1; ky <= 1; ky++) {
          for (let kx = -1; kx <= 1; kx++) {
            const px = ((y + ky) * width + (x + kx)) * 4;
            sum += copy[px + c] * kernel[ky + 1][kx + 1];
          }
        }
        const i = (y * width + x) * 4;
        data[i + c] = Math.min(Math.max(sum, 0), 255);
      }
    }
  }

  ctx.putImageData(imageData, 0, 0);
}
