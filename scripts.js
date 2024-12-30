console.log("Typescript loaded");
function elementToggle(el) {
    el.toggleAttribute("hidden");
    // change the style to include display
    // toggle element
}
function toBoard() {
    console.log("button pressed");
    var homeDiv = document.getElementById("homeDiv");
    if (homeDiv) {
        elementToggle(homeDiv);
    }
    var gameDiv = document.getElementById("gameDiv");
    if (gameDiv) {
        elementToggle(gameDiv);
    }
}
function toHome() {
    console.log("button pressed");
    var gameDiv = document.getElementById("gameDiv");
    if (gameDiv) {
        elementToggle(gameDiv);
    }
    var homeDiv = document.getElementById("homeDiv");
    if (homeDiv) {
        elementToggle(homeDiv);
    }
}
// TODO add functions for displaying board and displaying home page
// TODO add function to query engine
