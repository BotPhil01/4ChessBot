// holds display data for toggling visibility of divs
// NOTE!!! hidden gets overriden and visibility doesn't delete from document hence we have to do it this way
var displayData = {
    'homeDiv': 'centered',
    'gameDiv': ''
};
function elementToggle(elId) {
    var el = document.getElementById(elId);
    if (!el) {
        console.error("unable to get element from ".concat(elId));
        return;
    }
    if (el.getAttribute("class") == "destroy") {
        el.setAttribute("class", displayData[elId]);
    }
    else {
        el.setAttribute("class", "destroy");
    }
}
function swap() {
    console.log("button pressed");
    elementToggle("homeDiv");
    elementToggle("gameDiv");
}
// TODO add function to query engine
