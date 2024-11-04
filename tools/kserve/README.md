# Dev server to test out WASM projects

Features:
- Serves the target file by visiting "/"
- Enables SharedArrayBuffer and Wasm Web Workers by serving the proper headers
- Hot reloads target Emscripten projects when the .html file changes (may change this to the .js file of the same name)

Requires Node.js or Bun installed on your system.
No package dependencies, so there's no need to call `npm install`.

## Running the Server

```shell
node main.js html_file [other_files_to_watch...]
```

The target html file should have its necessary .js, .wasm, .data, etc. files stored relative to its own directory.
Now, by visiting http://localhost:3000, the target file should be properly served and hot-reloaded when changed.
Note: this server is not intended for a production environment

TODO: move to cpp!