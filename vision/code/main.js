let zoom = 1;

window.onload = () => {
    document.getElementById("zoomRange").oninput = OnZoomChange;
    document.getElementById("traceButton").onclick = OnDrawLine;
}

const OnOpenCVReady = () => {
    let imgElement = document.getElementById('imageSrc');
    let inputElement = document.getElementById('fileInput');
    inputElement.addEventListener('change', (e) => {
        imgElement.src = URL.createObjectURL(e.target.files[0]);
    }, false);
    imgElement.onload = function () {
        let mat = cv.imread(imgElement);
        cv.imshow('canvasOutput', mat);
        document.getElementById("canvasOutput").onclick = OnMouseCallback;
    };
    var Module = {
        // https://emscripten.org/docs/api_reference/module.html#Module.onRuntimeInitialized
        onRuntimeInitialized() {
            document.getElementById('status').innerHTML = 'OpenCV.js is ready.';
        }
    };
}

const OnZoomChange = (event) => {
    zoom = parseFloat(event.target.value);
    document.getElementById("zoomText").innerHTML = zoom;

    let imgElement = document.getElementById('imageSrc');
    let source = cv.imread(imgElement);
    let dest = new cv.Mat();
    let dsize = new cv.Size(source.size().width * zoom, source.size().height * zoom);

    cv.resize(source, dest, dsize, 0, 0);
    cv.imshow('canvasOutput', dest);
}

const OnDrawLine = () => {
    const [a, b, c] = [
        parseFloat(document.getElementById('aInput').value),
        parseFloat(document.getElementById('bInput').value),
        parseFloat(document.getElementById('cInput').value)
    ];

    let imgElement = document.getElementById('imageSrc');
    let source = cv.imread(imgElement);
    cv.line(source, p1, p2, [0, 255, 0, 255], 1);
    cv.imshow('canvasOutput', source);
}

const OnMouseCallback = (event) => {
    const rect = event.target.getBoundingClientRect();
    const [x, y] = [
        Math.floor((event.clientX - rect.left) / zoom),
        Math.floor((event.clientY - rect.top) / zoom)
    ];
    document.getElementById("clicked").innerHTML = `Clicked ${x}, ${y}`;
    let imgElement = document.getElementById('imageSrc');
    let source = cv.imread(imgElement);
    console.log(source.ucharPtr(y, x));
}
