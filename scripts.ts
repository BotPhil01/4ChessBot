console.log("Typescript loaded");

function elementToggle(el: HTMLElement) {
    
    // get element hidden attribute
    var hiddenVal: string | null = el.getAttribute("hidden");
    if (!hiddenVal || hiddenVal == "False") {
        el.setAttribute("hidden", "True");
    } else {
        el.setAttribute("hidden", "True");
    }
    // change the style to include display
    // toggle element
}

function boardButtonPress() {
    console.log("button pressed")
    const homeDiv: HTMLElement | null = document.getElementById("homeDiv");
    if (homeDiv) {
        elementToggle(homeDiv)
    }
}

// TODO add functions for displaying board and displaying home page
// TODO add function to query engine