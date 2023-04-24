var Module = {};

function start_app()
{
    var canvas = document.querySelector('canvas');
    canvas.width = 1280;
    canvas.height = 720;   
    Module.canvas = canvas;

    //check for WebAssembly support otherwise
    //fallback to using asm.js
    if(window.WebAssembly !== undefined)
    {
        var r = new XMLHttpRequest();
        r.open('GET', 'app_wasm.wasm', true);
        r.responseType = 'arraybuffer';
        r.onload = function() 
        {
            Module.wasmBinary = r.response;
            var script = document.createElement('script');
            script.src = 'app_wasm.js';
            document.body.appendChild(script);
        };
        r.send();
    }
    else
    {
        var script = document.createElement('script');
        script.src = 'app_fallback.js';
        document.body.appendChild(script);
    }
}

start_app();