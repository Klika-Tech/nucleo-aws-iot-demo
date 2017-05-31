/**
 * Optimized line path generator (d3).
 * @ref https://www.mapbox.com/osmdev/2012/11/20/getting-serious-about-svg/
 * @return function path generator
 * */
export default function lineOptimized() {
    let x = null;
    let y = null;

    function line(data) {
        return `M${data.map(a => [~~x(a), ~~y(a)]).join('L')}`;
    }

    line.x = (_) => {
        x = _;
        return line;
    };

    line.y = (_) => {
        y = _;
        return line;
    };

    return line;
}
