const fs               = require("node:fs");
const path             = require("node:path");
const fileServer       = require("./src/file-server.js");
const reloadServer     = require("./src/reload-server.js");
const util             = require("./src/util.js");

// ===== Constants ============================================================
const HOSTNAME = "127.0.0.1"; ///< servers will be hosted on localhost
const FILE_PORT = 3000;       ///< port number of the file server
const WS_PORT = 1234;         ///< port number of the websocket reload server

// ===== Global Variables =====================================================

/// @type {number}
/// Tracks the number of servers initialized to signal opening the web browser
let serversInit = 0;

/// Program entry point.
/// Args:
///     0: Node / Bun
///     1: Html file path
///     2: ... Additional files to watch for
/// @param {string[]} args command line arguments (0 is node, 1 is this filename)
function main(args)
{
    // Parse args
    if (args.length < 3)
    {
        console.log("Please provide the name of a file to serve");
        return -1;
    }

    let filename = args[2];
    if (!fs.existsSync(filename))
    {
        console.log("Requested file \"" + filename + "\" does not exist");
        return -2;
    }

    // @type {string[]}
    let otherFiles = [];
    for (let i = 3; i < args.length; ++i)
    {
        otherFiles.push(args[i]);
    }

    startServers(filename, otherFiles);
    return 0;
}

main(process.argv);

// ===== Helpers =================================================================

/// Passed to server open functions. When each server inits, it keeps track how
/// many have finished opening before running web browser with target file.
function serverInitCallback() {
    if (++serversInit >= 2)
        openBrowser();
}


/// @param {string} targetFile - target file to open/watch
/// @param {string[]} otherFiles - other files to watch for
function startServers(targetFile, otherFiles = [])
{
    // Make path to target file absolute
    if (!path.isAbsolute(targetFile))
    {
        targetFile = path.join(process.cwd(), targetFile);
    }

    // Make other paths absolute
    for (let i = 0; i < otherFiles.length; ++i)
    {
        if (!path.isAbsolute(otherFiles[i]))
        {
            otherFiles[i] = path.join(process.cwd(), otherFiles[i]);
        }
    }

    // Use target file directory as root
    const root = path.dirname(targetFile);
    try {
        otherFiles.push(targetFile);
        fileServer.open(HOSTNAME, FILE_PORT, root, targetFile, serverInitCallback);
        reloadServer.open(HOSTNAME, WS_PORT, otherFiles, serverInitCallback);
    }
    catch(err)
    {
        shutdown();
        throw err;
    }

    process.on("SIGTERM", () => {
        shutdown();
        process.exit(0);
    });

    process.on("SIGINT", () => {
        shutdown();
        process.exit(0);
    });

    return 0;
}

function shutdown()
{
    console.log("Shutting down");

    reloadServer.close();
    fileServer.close();
    serversInit = 0;
}

/// Open the file browser at the root of the file server
function openBrowser()
{
    util.openURL("http://localhost:" + FILE_PORT);
}
