# Dev server to test out WASM projects

Features:
- Serves the target file at http://localhost:3000
- Enables `SharedArrayBuffer` and Wasm Web Workers by serving the proper security headers
- Hot-reloads target project when the files you indicate changes

Requires Node.js or Bun installed on your system.
No package dependencies, so there's no need to call `npm install`.

## Running the Server

```shell
node index.js html_file [other_files_to_watch,...]
```

The target html file should have its necessary .js, .wasm, .data, etc. files stored relative to its own directory.
Now, by visiting http://localhost:3000, the target file should be properly served and hot-reloaded when changed.
Note: this server is not intended for a production environment

TODO: move to cpp! Remove dependency on node/Bun.
