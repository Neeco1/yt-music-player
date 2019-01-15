var btnPrev, btnPlay, btnStop, btnPause, btnNext;
var circleDiv;
var overlay, overlayClose;
var playlistNameInput, playlistUrlInput;
var searchBtn, closeSearchOverlay, searchText, globalSearchTimer;
var titleDiv, designSelector, flatBtn, roundBtn, blocks;
var volSlider;
var newPlaySendBtn, newPlayForm;

/******************************************************************************
 * GUI Initialization
 ******************************************************************************/
function gui_init() {
    playlistNameInput = document.getElementById("playlist_name");
    playlistUrlInput = document.getElementById("playlist_url");

    gui_initButtons();
    gui_initCircleDiv();
    gui_initOverlay();
    //Search button + overlay
    gui_initSearch();
    //Design selection component
    gui_initDesignSelection();
    //Volume slider component
    gui_initVolSlider();
    //Overlay button component
    gui_initOverlayButtonComponent();
    //Playlist overlay component
    var closePlaylistOverlay = document.getElementById("closePlaylistOverlay");
    closePlaylistOverlay.addEventListener("click", function() {
        gui_hideOverlay("playlist_overlay");
    });
    //Progress bar event handlers
    var progressContainer = document.getElementById("progress_container");
    progressContainer.addEventListener("click", handleProgressClick, true);
    //playback info click handler
    var playbackInfoContent = document.getElementById("playback_info_content");
    playbackInfoContent.addEventListener("click", function() {
        gui_loadPlaylistDataToOverlay(playlists[currentPlaylistId]);
        gui_showOverlay("playlist_overlay");
    });
    //Add key listeners
    gui_addKeyListeners();
}

function gui_initButtons() {
    btnPrev = document.getElementById("btnPrev");
    btnPause = document.getElementById("btnPause");
    btnStop = document.getElementById("btnStop");
    btnPlay = document.getElementById("btnPlay");
    btnNext = document.getElementById("btnNext");
}
function gui_initCircleDiv() {
    circleDiv = document.getElementById("circle");
    circleDiv.addEventListener("touchstart", function()
    {
        circleDiv.style.backgroundColor = "#CCA83B";
    });
    circleDiv.addEventListener("mousedown", function()
    {
        circleDiv.style.backgroundColor = "#CCA83B";
    });
    circleDiv.addEventListener("mouseup", function()
    {
        circleDiv.style.backgroundColor = "#FFB035";
    });
    circleDiv.addEventListener("click", function() {
        overlay.style.display="block";
    });
}
function gui_closeOverlay() {
    overlay.style.display = "none";
    playlistNameInput.value = "";
    playlistUrlInput.value = "";
}

function gui_initOverlay() {
    overlay = document.getElementById("overlay");
    overlayClose = document.getElementById("overlay_close");
    overlayClose.addEventListener("click", gui_closeOverlay);
}
function gui_initSearch() {
    searchBtn = document.getElementById("search_btn");
    searchBtn.addEventListener("click", function() {
        if(document.getElementById("search_overlay").classList.contains("visible"))
        {
            gui_hideOverlay("search_overlay");
            return;
        }
        gui_showOverlay("search_overlay");
    });
    closeSearchOverlay = document.getElementById("closeSearchOverlay");
    closeSearchOverlay.addEventListener("click", function() {
        gui_hideOverlay("search_overlay");
    });
    searchText = document.getElementById("search_text");
    searchText.addEventListener("keyup", function(e) {
        globalSearchTimer = setTimeout(yt_search, 500);
        if(e.key == "Enter") {
            document.activeElement.blur();
        }
    });
    searchText.addEventListener("keydown", function() {
        if(globalSearchTimer)
            clearTimeout(globalSearchTimer)
    });
}
function gui_initDesignSelection() {
    titleDiv = document.getElementById("title");
    designSelector = document.getElementById("design_selector");
    flatBtn = document.getElementById("design_flat");
    roundBtn = document.getElementById("design_round");
    blocks = document.getElementsByClassName("playlist_block");
    title.addEventListener("click", function() { designSelector.style.display="block"; });
    flatBtn.addEventListener("click", function() {
        Array.from(blocks).forEach(elm => { elm.classList.remove("block_round") });
        designSelector.style.display="none";
    });
    roundBtn.addEventListener("click", function() {
        Array.from(blocks).forEach(elm => {
            if(!elm.classList.contains("block_round"))
                elm.classList.add("block_round")
        });
        designSelector.style.display="none";
    });
}
function gui_initVolSlider() {
    volSlider = document.getElementById("volume_slider");
    var volSliderFunction = function() {
        wsConn.send('{ "cmd" : "set_volume", "data" : {"volume":'+volSlider.value+'} }');
    }
    volSlider.addEventListener("touchend", volSliderFunction);
    volSlider.addEventListener("mouseup", volSliderFunction);
}
function gui_initOverlayButtonComponent() {
    newPlaySendBtn = document.getElementById("sendform_button");
    newPlayForm = document.getElementById("newPlayForm");
    var sendPlaylistURL = function() {
        newPlaySendBtn.setAttribute("disabled", "disabled");
        var playlistUrl = playlistUrlInput.value.trim();
        var playlistName = playlistNameInput.value.trim();
        wsConn.send('{ "cmd" : "add_playlist", '
            + '"data" : {"playlist_url":"'+playlistUrl+'", "name":"'+playlistName+'"} }');
        gui_closeOverlay();
        newPlaySendBtn.removeAttribute("disabled");
    }
    newPlayForm.addEventListener("submit", function(e) {
        sendPlaylistURL();
        e.preventDefault();
        return false;
    });
    newPlaySendBtn.removeAttribute("disabled");
}
function gui_addKeyListeners() {
    document.onkeydown = function(evt) {
        evt = evt || window.event;
        var isEscape = false;
        if ("key" in evt) {
            isEscape = (evt.key == "Escape" || evt.key == "Esc");
        } else {
            isEscape = (evt.keyCode == 27);
        }
        if (isEscape) {
            gui_hideOverlay("playlist_overlay");
        }
    };
}

/*****************************************************************************
 * GUI Logic
 *****************************************************************************/

function gui_updatePlaybackStarted() {
    gui_showNowPlaying();
    gui_updateButtons(false, true, false, false, false);
}

function gui_updatePlaybackStopped() {
    try
    {
        gui_hideNowPlaying();
        document.getElementById("playlist"+currentPlaylistId).classList.remove("now_playing");
        gui_updateButtons(false, false, true, false, false);
        return;
    }
    catch(e) {}
}

function gui_showNowPlaying() {
    document.getElementById("button_container").style.marginBottom = "60px";
    document.getElementById("playback_info").style.marginBottom="0px";
}

function gui_hideNowPlaying() {
    document.getElementById("button_container").style.marginBottom = "0px";
    document.getElementById("playback_info").style.marginBottom="-60px";
}

function gui_showNotification(message) {
    //TODO
}

function gui_updateButtons(prev, play, stop, pause, next) {
    if(!prev) btnPrev.classList.remove("stateActive");
    else btnPrev.classList.add("stateActive");

    if(!play) btnPlay.classList.remove("stateActive");
    else btnPlay.classList.add("stateActive");

    if(!stop) btnStop.classList.remove("stateActive");
    else btnStop.classList.add("stateActive");

    if(!pause) btnPause.classList.remove("stateActive");
    else btnPause.classList.add("stateActive");

    if(!next) btnNext.classList.remove("stateActive");
    else btnNext.classList.add("stateActive");
}

function gui_updateNowPlaying(list_id, now_playing) {
    var list_elm = document.getElementById("playlist"+list_id);
    if(now_playing)
        list_elm.classList.add("now_playing");
    else
        list_elm.classList.remove("now_playing");
}

function gui_createLoadingDom() {
    var progressDiv = document.createElement("div");
    progressDiv.setAttribute("class", "progress_overlay");
    var loaderDiv1 = document.createElement("div");
    loaderDiv1.classList.add("lds-css");
    loaderDiv1.classList.add("ng-scope");
    var loaderDiv2 = document.createElement("div");
    loaderDiv2.style.width = "60px";
    loaderDiv2.style.height = "60px";
    loaderDiv2.classList.add("lds-eclipse");
    loaderDiv2.appendChild(document.createElement("div"));
    loaderDiv1.appendChild(loaderDiv2);
    progressDiv.appendChild(loaderDiv1);
    return progressDiv;
}

function gui_playlistToDom(playlist, in_progress) {
    var blockDiv = document.createElement("div");
    blockDiv.setAttribute("class", "playlist_block");
    blockDiv.addEventListener("click", function() {
        gui_loadPlaylistDataToOverlay(playlist);
        gui_showOverlay("playlist_overlay");
    });

    if(playlist.playing)
    {
        currentPlaylistId = playlist.id;
        blockDiv.classList.add("now_playing");
    }
    blockDiv.setAttribute("id", "playlist"+playlist.id);
    blockDiv.style.backgroundColor = getRandomColor();

    if(in_progress) {
        var progressDiv = gui_createLoadingDom();
        blockDiv.appendChild(progressDiv);
    }

    var subDiv = document.createElement("div");
    subDiv.setAttribute("class", "block_content");
    var link = document.createElement("a");
    link.setAttribute("class", "title");
    var span = document.createElement("span");
    span.innerHTML = playlist.name;
    link.appendChild(span);
    subDiv.appendChild(link);
    blockDiv.appendChild(subDiv);

    return blockDiv;
}

function gui_showPlaylists() {
    blockContainer.innerHTML = "";
    for (var list_id in playlists)
    {
        blockContainer.appendChild(gui_playlistToDom(playlists[list_id], false));
    }
}

function gui_setPlaybackMode(mode) {
    if(mode === "repeat") 
    {
        document.getElementById("btnRepeat").classList.remove("hidden");
        document.getElementById("btnShuffle").classList.add("visible");
    }
    if(mode === "shuffle") 
    {
        document.getElementById("btnRepeat").classList.add("hidden");
        document.getElementById("btnShuffle").classList.remove("visible");
    }
}

function gui_loadPlaylistDataToOverlay(playlist) {
    function createTrackRow(track, trackIndex) {
        var row = document.createElement("div");
        row.classList.add("track_row");
        if(track.id == playbackInfo.track_id) {
            row.classList.add("playing");
        }
        row.addEventListener("click", function() {
            playTrackFromList(playlist.id, trackIndex);
        });

        //Track image
        var imgWrapper = document.createElement("div");
        imgWrapper.classList.add("img_wrapper");
        var img = document.createElement("img");
        img.setAttribute("src", track.thumbUrl);
        imgWrapper.appendChild(img)
        row.appendChild(imgWrapper);
        //Track name
        var span = document.createElement("span");
        span.innerHTML = track.name;
        row.appendChild(span);
        //Track duration
        var durationDiv = document.createElement("div");
        durationDiv.classList.add("track_duration");
        var date = new Date(); 
        date.setTime((track.duration.h*3600 + track.duration.m*60 + track.duration.s) * 1000);
        var strDuration = "";
        if(track.duration.h != 0) 
        { 
            var strDuration = ('0' + date.getHours()).slice(-2) + ':';
        }
        strDuration +=  ('0' + date.getMinutes()).slice(-2) + ':' + ('0' + date.getSeconds()).slice(-2);
        durationDiv.innerHTML = strDuration;
        row.appendChild(durationDiv)

        return row;
    } 
 
    var numTracks = playlist.tracks.length;

    var divTitle = document.getElementById("playlistoverlay_title");
    var divTracks = document.getElementById("playlistoverlay_tracks");
    //Clear track div
    divTitle.innerHTML = playlist.name + " (" + numTracks + ")";
    divTracks.innerHTML = "";

    //List all tracks
    var tracks = playlist.tracks;
    for (var i = 0; i < tracks.length; i++)
    {
        var track = tracks[i];
        divTracks.appendChild(createTrackRow(track, i));
    }
}

function gui_showOverlay(css_id) {
    var overlay = document.getElementById(css_id);
    overlay.classList.add("visible");
}
function gui_hideOverlay(css_id) {
    var overlay = document.getElementById(css_id);
    overlay.classList.remove("visible");
}
