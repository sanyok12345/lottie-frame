# Lottie Frame

## Description

`lottie-frame` is a Node.js native addon that allows you to export frames from Lottie animations to PNG images. It leverages the `rlottie` library for rendering and `libpng` for PNG encoding, providing a fast and efficient way to extract frames from Lottie animations.

## Features

-   Asynchronous and synchronous frame export
-   Configurable frame number, width, height, and quality
-   Native C++ implementation for performance
-   Easy to use Node.js API

## Installation

```bash
npm install lottie-frame
```

## Requirements

-   Node.js (version 20 or higher)
-   `rlottie` and `libpng` must be installed on your system.

    -   **Ubuntu:**

        ```bash
        sudo apt-get update && sudo apt-get install -y libpng-dev librlottie-dev
        ```

    -   **macOS:**

        ```bash
        brew install libpng rlottie
        ```

    -   **Windows:**

        -   Requires `python` and `node-gyp`.

## Usage

```javascript
import { exportFrame, exportFrameSync } from 'lottie-frame';
import fs from 'node:fs';

// Load your Lottie animation file
const lottieFile = fs.readFileSync('path/to/your/animation.json');

// Asynchronous example
async function exportLottieFrameAsync() {
  try {
    const pngBuffer = await exportFrame(lottieFile, {
      frame: 30,
      width: 512,
      height: 512,
      quality: 80,
    });

    fs.writeFileSync('frame_async.png', pngBuffer);
    console.log('PNG file saved: frame_async.png');
  } catch (error) {
    console.error('Error exporting frame:', error);
  }
}

exportLottieFrameAsync();

// Synchronous example
try {
  const pngBufferSync = exportFrameSync(lottieFile, {
    frame: 30,
    width: 512,
    height: 512,
    quality: 80,
  });

  fs.writeFileSync('frame_sync.png', pngBufferSync);
  console.log('PNG file saved: frame_sync.png');
} catch (error) {
  console.error('Error exporting frame:', error);
}
```

## API

### `exportFrame(buffer: Buffer, options?: Options): Promise<Buffer>`

Asynchronously exports a frame from a Lottie animation to PNG.

-   `buffer`: The Lottie JSON animation data as a Buffer.
-   `options`: (Optional) Configuration options for the export.
    -   `frame`: The frame number to export (non-negative integer, default: 0).
    -   `width`: The width of the exported PNG (integer between 1 and 4096, default: 100).
    -   `height`: The height of the exported PNG (integer between 1 and 4096, default: 100).
    -   `quality`: The PNG compression quality (integer between 1 and 100, default: 100).

Returns: A promise resolving to the PNG data as a Buffer.

Throws:

-   `TypeError`: If `buffer` is not a Buffer.
-   `RangeError`: If any option value is invalid.
-   `Error`: If the native module fails to process the animation.

### `exportFrameSync(buffer: Buffer, options?: Options): Buffer`

Synchronously exports a frame from a Lottie animation to PNG.

-   `buffer`: The Lottie JSON animation data.
-   `options`: (Optional) Configuration options for the export.
    -   `frame`: The frame number to export (non-negative integer, default: 0).
    -   `width`: The width of the exported PNG (integer between 1 and 4096, default: 100).
    -   `height`: The height of the exported PNG (integer between 1 and 4096, default: 100).
    -   `quality`: The PNG compression quality (integer between 1 and 100, default: 100).

Returns: The PNG data as a Buffer.

Throws:

-   `TypeError`: If `buffer` is not a Buffer.
-   `RangeError`: If any option value is invalid.
-   `Error`: If the native module fails to process the animation.

## Building

```bash
npm install
npm run build
```

## Prebuilding

```bash
npm install
npm run prebuild
```

## License

MIT