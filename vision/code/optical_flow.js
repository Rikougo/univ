class BoundingBox {
    /**
     * 
     * @param {{x: number, y: number}} p_topLeft
     * @param {{x: number, y: number}} p_bottomRight
     */
    constructor(p_topLeft, p_bottomRight) {
        this.topLeft = p_topLeft;
        this.bottomRight = p_bottomRight;
    }

    width() {
        return this.topLeft.x - this.bottomRight.x;
    }

    height() {
        return this.topLeft.y - this.bottomRight.y;
    }

    translate(u, v) {
        return new BoundingBox(
            {x: this.topLeft.x + u, y: this.topLeft.y + v},
            {x: this.bottomRight.x + u, y: this.bottomRight.y + v},
        );
    }
}

const ExtracPattern = (p_source, p_boundingBox) => {
    let pattern = new cv.Mat(p_boundingBox.height(), p_boundingBox.width(), p_source.type());

    for(let i = p_boundingBox.bottomRight.x; i < p_boundingBox.topLeft.x; i++) {
        for(let j = p_boundingBox.bottomRight.y; j < p_boundingBox.topLeft.y; j++) {
            let sI = i - p_boundingBox.bottomRight.x, sJ = j - p_boundingBox.bottomRight.y;
            pattern.data[sI + sJ * pattern.cols] = p_source.data[i + j * p_source.cols];
        }
    }

    return pattern;
};

const Flatten = (p_source) => {
    let l_flatten = new cv.Mat(p_source.rows * p_source.cols, 1, p_source.type());

    for(let i = 0; i < p_source.rows * p_source.cols; i++) l_flatten.data[i] = p_source.data[i];

    return l_flatten;
}

const MatMul = (p_A, p_B) => {
    if (p_A.size().width != p_B.size().height) {
        console.error("Incompatible size", p_A.size(), p_B.size());
        return;
    }

    let l_result = new cv.Mat(p_A.rows, p_B.cols, p_A.type());
    for(let i = 0; i < l_result.rows * l_result.cols; i++) l_result.data[i] = 0;

    for (let i = 0; i < p_A.rows; i++) {
        for (let k = 0; k < p_B.cols; k++) {
            for (let j = 0; j < p_A.cols; j++) {
                l_result.data[i * l_result.cols + k] += p_A.data[i * p_A.cols + j] * p_B.data[j * p_B.cols + k];
            }
        }
    }

    return l_result;
}

/**
 * 
 * @param {} p_previousImg 
 * @param {} p_nextImg 
 * @param {BoundingBox} p_boundingBox 
 */
const OpticalFlow = (p_ApseudoInvert, p_nextImg, p_boundingBox, p_baseBattern) => {
    let l_extractedPattern = ExtracPattern(p_nextImg, p_boundingBox);

    let l_B = new cv.Mat();
    let l_mask = new cv.Mat();
    let l_dtype = cv.CV_32F;
    cv.subtract(l_extractedPattern, p_baseBattern, l_B, l_mask, l_dtype);
    
    let l_flatB = Flatten(l_B);

    console.log(l_flatB.size(), p_ApseudoInvert.size(), l_flatB.type(), p_ApseudoInvert.type(), cv.CV_32F);

    let l_UV = MatMul(p_ApseudoInvert, l_flatB);
    console.log(l_UV.data[0], l_UV.data[1]);
    return p_boundingBox.translate(l_UV.data[0], l_UV.data[1]);
}