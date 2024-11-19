package com.kaze.app;

import org.libsdl.app.SDLActivity;

import android.os.Bundle;

/// Example Activity implementation.
/// @note In this example, we're using SDL3 for windowing and input.
///       Our Activity inherits from SDLActivity since SDL3 requires a lot of glue code to function.
///       To run our `int main`, it requires our code to be loaded in a shared library with the
//        specific name "main".
///       If you're handling windowing and input by other means you don't need SDLActivity at all,
///       but will need to load your library yourself.
public class KazeActivity extends SDLActivity {
    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);

        /// Initialize the Kaze's native runtime code
        com.kaze.app.Kaze.init(this);
    }

    @Override
    protected void onDestroy()
    {
        /// Clean up Kaze's native runtime code
        com.kaze.app.Kaze.close();

        super.onDestroy();
    }
}
