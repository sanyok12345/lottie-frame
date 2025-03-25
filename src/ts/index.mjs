import { Buffer } from "node:buffer";
import { join } from "node:path";
import { fileURLToPath } from "node:url";

const moduleDir = join(fileURLToPath(import.meta.url), "..");

/**
 * Options for exporting a frame from a Lottie animation.
 * @typedef {Object} Options
 * @property {number} [frame] - The frame number to export (non-negative integer, default: 0).
 * @property {number} [width] - The width of the exported PNG (integer between 1 and 4096, default: 100).
 * @property {number} [height] - The height of the exported PNG (integer between 1 and 4096, default: 100).
 * @property {number} [quality] - The PNG compression quality (integer between 1 and 100, default: 100).
 */

/**
 * Loads the native Lottie module based on the platform and environment.
 * @returns {NativeModule} The loaded native module.
 * @throws {Error} If the native module cannot be loaded.
 */
function loadNativeModule() {
    const extension = process.platform === "win32" ? ".node" : ".node";
    const buildType = process.env.NODE_ENV === "debug" ? "Debug" : "Release";
    const modulePath = join(
        moduleDir,
        "..",
        "build",
        buildType,
        `lottie-frame${extension}`
    );

    try {
        const module = { exports: {} };
        process.dlopen(module, modulePath);
        return module.exports;
    } catch (error) {
        throw new Error(
            `Failed to load native module at ${modulePath}: ${error instanceof Error ? error.message : String(error)
            }`
        );
    }
}

const native = loadNativeModule();

/**
 * Asynchronously exports a frame from a Lottie animation to PNG.
 * @param {Buffer} buffer - The Lottie JSON animation data.
 * @param {Options} [options] - Configuration options for the export.
 * @returns {Promise<Buffer>} A promise resolving to the PNG data as a Buffer.
 * @throws {TypeError} If `buffer` is not a Buffer.
 */
export async function exportFrame(buffer, options) {
    validateInput(buffer, options);
    return native.exportFrame(buffer, options);
}

/**
 * Synchronously exports a frame from a Lottie animation to PNG.
 * @param {Buffer} buffer - The Lottie JSON animation data.
 * @param {Options} [options] - Configuration options for the export.
 * @returns {Buffer} The PNG data as a Buffer.
 * @throws {TypeError} If `buffer` is not a Buffer.
 */
export function exportFrameSync(buffer, options) {
    validateInput(buffer, options);
    return native.exportFrameSync(buffer, options);
}

/**
 * Validates the input parameters for exporting a Lottie frame.
 * @param {Buffer} buffer - The Lottie JSON animation data.
 * @param {Options} [options] - Configuration options for the export.
 * @throws {TypeError} If `buffer` is not a Buffer.
 */
function validateInput(buffer, options) {
    if (!Buffer.isBuffer(buffer)) {
        throw new TypeError("Input must be a Buffer containing Lottie JSON data");
    }

    if (options) {
        if (typeof options.frame === "number") {
            if (!Number.isInteger(options.frame) || options.frame < 0) {
                throw new RangeError('Option "frame" must be a non-negative integer');
            }
        }

        if (typeof options.width === "number") {
            if (
                !Number.isInteger(options.width) ||
                options.width < 1 ||
                options.width > 4096
            ) {
                throw new RangeError(
                    'Option "width" must be an integer between 1 and 4096'
                );
            }
        }

        if (typeof options.height === "number") {
            if (
                !Number.isInteger(options.height) ||
                options.height < 1 ||
                options.height > 4096
            ) {
                throw new RangeError(
                    'Option "height" must be an integer between 1 and 4096'
                );
            }
        }

        if (typeof options.quality === "number") {
            if (
                !Number.isInteger(options.quality) ||
                options.quality < 1 ||
                options.quality > 100
            ) {
                throw new RangeError(
                    'Option "quality" must be an integer between 1 and 100'
                );
            }
        }
    }
}