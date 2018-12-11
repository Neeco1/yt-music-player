var gapikey = 'AIzaSyCKMpw2nmPnon_gkh4EIXnbiAmrZNw-v4M';

function yt_init() {    
    searchField = $("#search_text");
    div_results = $("#search_overlay_results");
    div_buttons = $("#search_overlay_buttons");
    
    //Focus event handler
    searchField.on('focus', function() {
    });
    
    // Blur event handler
    searchField.on('blur', function() {
    }); 
}

function yt_search() {
    div_results.html('');
    div_buttons.html('');
    var query = searchField.val();

    $.get(
    	"https://www.googleapis.com/youtube/v3/search", {
            part: 'snippet, id',
            q: query,
            type: 'video',
            key: gapikey
        }, 
        function(data) 
        {
            div_results.html('');
            div_buttons.html('');
            $.each(data.items, function(i, item) {
                div_results.append(yt_getOutput(item));
            });
            var buttons = yt_getButtons(data.prevPageToken, data.nextPageToken, query);
            div_buttons.append(buttons);
        });
}

function yt_nextPage() {
    var token = $('#next-button').data('token');
    div_results.html('');
    div_buttons.html('');
    var query = searchField.val();
    
    $.get(
    	"https://www.googleapis.com/youtube/v3/search", {
            part: 'snippet, id',
            q: query,
            pageToken: token,
            type: 'video',
            key: gapikey
        }, 
        function(data) 
        {
            div_results.html('');
            div_buttons.html('');
            $.each(data.items, function(i, item) {
                div_results.append(yt_getOutput(item));
            });
            var buttons = yt_getButtons(data.prevPageToken, data.nextPageToken, query);
            div_buttons.append(buttons);
        });    
}

function yt_prevPage() {
    var token = $('#prev-button').data('token');
    div_results.html('');
    div_buttons.html('');
    var query = searchField.val();
    
    $.get(
    	"https://www.googleapis.com/youtube/v3/search", {
            part: 'snippet, id',
            q: query,
            pageToken: token,
            type: 'video',
            key: gapikey
        }, 
        function(data) 
        {
            div_results.html('');
            div_buttons.html('');
            $.each(data.items, function(i, item) {
                var output = yt_getOutput(item);
                div_results.append(output);
            });
            var buttons = yt_getButtons(data.prevPageToken, data.nextPageToken, query);
            div_buttons.append(buttons);
        });    
}

function yt_getOutput(item) {
    var videoID = item.id.videoId;
    var title = item.snippet.title;
    var description = item.snippet.description;
    var thumb = item.snippet.thumbnails.high.url;
    var channelTitle = item.snippet.channelTitle;
    var videoDate = item.snippet.publishedAt;
    
    var url = "https://youtube.com/watch?v="+videoID

    var output = 	'<li>' +
        				'<div class="list-left">' +
        					'<img src="' + thumb + '">' +
        				'</div>' +
        				'<div class="list-right" onclick="playMediaFromURL(\''+url+'\')">' +
        					'<h3><a>' + title + '</a></h3>' +
        					'<small>By <span class="cTitle">' + channelTitle + '</span> on ' + videoDate + '</small>' +
        					'<p>' + description + '</p>' +
        				'</div>' +
        			'</li>' +
        			'<div class="clearfix"></div>' +
        			'';
    return output;
}

function yt_getButtons(prevPageToken, nextPageToken, query) {
    if(!prevPageToken) {
        var btnoutput = 	'<div class="search-btn-container">' +
            					'<button id="next-button" class="paging-button" data-token="' + nextPageToken + '" data-query="' + query + '"' +
            						'onclick = "yt_nextPage();">Next Page</button>' +
            				'</div>';
    } else {
        var btnoutput = 	'<div class="search-btn-container">' +
								'<button id="prev-button" class="paging-button" data-token="' + prevPageToken + '" data-query="' + query + '"' +
            						'onclick = "yt_prevPage();">Prev Page</button>' +            
            					'<button id="next-button" class="paging-button" data-token="' + nextPageToken + '" data-query="' + query + '"' +
            						'onclick = "yt_nextPage();">Next Page</button>' +
            				'</div>';        
    }
    
    return btnoutput;
}