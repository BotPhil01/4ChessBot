console.log("Typescript loaded");

function elementToggle(el: HTMLElement) {
    el.toggleAttribute("hidden");
    // change the style to include display
    // toggle element
}

function toBoard() {
    console.log("button pressed")
    const homeDiv: HTMLElement | null = document.getElementById("homeDiv");
    if (homeDiv) {
        elementToggle(homeDiv)
    }
    const gameDiv: HTMLElement | null = document.getElementById("gameDiv");
    if (gameDiv) {
        elementToggle(gameDiv)
    }
}

function toHome() {
    console.log("button pressed")
    const gameDiv: HTMLElement | null = document.getElementById("gameDiv");
    if (gameDiv) {
        elementToggle(gameDiv)
    }
    const homeDiv: HTMLElement | null = document.getElementById("homeDiv");
    if (homeDiv) {
        elementToggle(homeDiv)
    }
}

// TODO add functions for displaying board and displaying home page
// TODO add function to query engine