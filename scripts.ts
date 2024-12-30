// holds display data for toggling visibility of divs
// NOTE!!! hidden gets overriden and visibility doesn't delete from document hence we have to do it this way
var displayData = {
    'homeDiv':'initial',
    'gameDiv':'grid'
}

function elementToggle(elId: string) {
    let el = document.getElementById(elId);
    if (!el) {
        console.error(`unable to get element from ${elId}`);
        return
    }
    if (window.getComputedStyle(el).display == "none") {
        el.style.display = (displayData[elId]);
    } else {
        el.style.display = ("none");
    }
}

function swap() {
    console.log("button pressed")
    elementToggle("homeDiv");
    elementToggle("gameDiv");
}

// TODO add function to query engine