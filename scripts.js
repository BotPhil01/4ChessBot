// holds display data for toggling visibility of divs
// NOTE!!! hidden gets overriden and visibility doesn't delete from document hence we have to do it this way
var displayData = {
    'homeDiv': 'initial',
    'gameDiv': 'grid'
};
function elementToggle(elId) {
    var el = document.getElementById(elId);
    if (!el) {
        console.error("unable to get element from ".concat(elId));
        return;
    }
    // console.log(`elid: ${elId}`);
    // console.log(window.getComputedStyle(el).display);
    if (window.getComputedStyle(el).display == "none") {
        el.style.display = (displayData[elId]);
    }
    else {
        el.style.display = ("none");
    }
    // change the style to include display
    // toggle element
}
function swap() {
    console.log("button pressed");
    elementToggle("homeDiv");
    elementToggle("gameDiv");
}
// TODO add functions for displaying board and displaying home page
// TODO add function to query engine
