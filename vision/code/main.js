let zoom = 1;
let images = [];
let currentCVImage;

let topleftbb = {x: 0, y: 0};
let bottomrightbb = {x: 0, y: 0};

let validated = false;
let bb;
let selected_pattern;
let A;
let A_invert;

const OnOpenCVReady = () => {
    document.getElementById('status').innerHTML = 'OpenCV.js is ready.';

    let imgElement = document.getElementById('imageSrc');
    let inputElement = document.getElementById('fileInput');

    let validateButtonElement = document.getElementById('validateButton');
    validateButtonElement.addEventListener("click", InitTracking);
    
    /**
     * @type{HTMLInputElement}
     */
    let imageSliderElement = document.getElementById('imgSlider');
    imageSliderElement.disabled = true;
    inputElement.addEventListener('change', (e) => {
        inputElement.disabled = true;
        images = e.target.files;
        imgElement.src = URL.createObjectURL(images[0]);

        imageSliderElement.min = 0; imageSliderElement.max = images.length - 1;
        imageSliderElement.value = 0;
        imageSliderElement.addEventListener('change', ChangeImage);
    }, false);

    imgElement.onload = function () {
        UpdateCVImage();
        
        if (validated) {   
            bb = OpticalFlow(A_invert, currentCVImage, bb, selected_pattern);

            DrawSelectedRectangleOnCVImage();
        }
        
        ShowCurrentImage();

        if (!validated) document.getElementById("canvasOutput").onclick = OnMouseCallback;
    };
}

const UpdateCVImage = () => {
    let imgElement = document.getElementById('imageSrc');
    let source = cv.imread(imgElement);
    currentCVImage = new cv.Mat();
    cv.cvtColor(source, currentCVImage, cv.COLOR_RGBA2GRAY, 0); 
}

const DrawSelectedRectangleOnCVImage = () => {
    console.log(bb);
    cv.rectangle(currentCVImage, bb.topLeft, bb.bottomRight, new cv.Scalar(255, 0, 0, 255));
}

const ShowCurrentImage = () => {
    cv.imshow('canvasOutput', currentCVImage);
}

const InitTracking = (p_event) => {
    console.log("Validating selection...");
    validated = true;
    document.getElementById('validateButton').disabled = true;
    document.getElementById("clicked").innerHTML = "";


    /**
     * @type{HTMLInputElement}
     */
     let imageSliderElement = document.getElementById('imgSlider');
     imageSliderElement.disabled = false;

    console.log("Creating bouding box...")
    bb = new BoundingBox(topleftbb, bottomrightbb);

    console.log("Extract pattern...")
    selected_pattern = ExtracPattern(currentCVImage, bb);

    cv.imshow('patternPreview', selected_pattern);

    console.log("Compute A inverse...");
    let dstx = new cv.Mat();
    let dsty = new cv.Mat();

    cv.Sobel(selected_pattern, dstx, cv.CV_8U, 1, 0, 3, 1, 0, cv.BORDER_DEFAULT);
    cv.Sobel(selected_pattern, dsty, cv.CV_8U, 0, 1, 3, 1, 0, cv.BORDER_DEFAULT);

    cv.imshow('gradientXPreview', dstx);
    cv.imshow('gradientYPreview', dsty);

    dstx = Flatten(dstx);
    dsty = Flatten(dsty);
    let gradient = new cv.Mat(dstx.rows, 2, dstx.type());

    for(let i = 0; i < dstx.rows; i++) {
        gradient.data[i*2] = dstx.data[i];
        gradient.data[i*2+1] = dsty.data[i];
    }

    console.log(dstx.size(), dsty.size(), gradient.size());

    A = gradient;
    A_invert = new cv.Mat();
    let _32fmat = new cv.Mat();
    A.convertTo(_32fmat, cv.CV_32F);
    cv.invert(_32fmat, A_invert, cv.DECOMP_SVD);
    console.log(A_invert.size());

    console.log("Tracking initialized.");
}

const ChangeImage = (p_event) => {
    let imgElement = document.getElementById('imageSrc');
    imgElement.src = URL.createObjectURL(images[parseInt(p_event.target.value)]);
}

const OnMouseCallback = (event) => {
    if (!validated) {
        const rect = event.target.getBoundingClientRect();
        const [x, y] = [
            Math.floor((event.clientX - rect.left)),
            Math.floor((event.clientY - rect.top))
        ];
    
        bottomrightbb = topleftbb;
        topleftbb = {x: x, y: y};

        bb = new BoundingBox(topleftbb, bottomrightbb);
        console.log(bb);
    
        document.getElementById("clicked").innerHTML = `Clicked ${x}, ${y}`;
    
        UpdateCVImage();
        DrawSelectedRectangleOnCVImage();
        ShowCurrentImage();
    }
}
