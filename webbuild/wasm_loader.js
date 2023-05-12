var Module = {};

function start_app()
{
    var canvas = document.querySelector('canvas');
    canvas.width = 1280;
    canvas.height = 720;   
    Module.canvas = canvas;
    if (!("arguments" in Module)){
        Module["arguments"] = [];
    }
    Module["arguments"].push("assets/sprite.spr");
    Module["arguments"].push("assets/palette.pal");

    //check for WebAssembly support otherwise
    //fallback to using asm.js
    if(window.WebAssembly !== undefined)
    {
        var r = new XMLHttpRequest();
        r.open('GET', app + '.wasm', true);
        r.responseType = 'arraybuffer';
        r.onload = function() 
        {
            Module.wasmBinary = r.response;
            var script = document.createElement('script');
            script.src = app + '.js';
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