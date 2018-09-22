import youtube_dl
import json

ydl = youtube_dl.YoutubeDL({'outtmpl': '%(id)s%(ext)s', 'quiet':True,})

def fetchTrackInfo(yt_url):
    result = ydl.extract_info(yt_url, download=False)

    videoEntry = "";

    if 'entries' in result:
        # Can be a playlist or a list of videos
        videoEntry = result['entries'][0]
    else:
        videoEntry = result        

    return json.dumps(videoEntry)


def fetchPlaylistInfo(yt_url):
    yt_result = ydl.extract_info(yt_url, download=False)

    results = []  
    if 'entries' in result:

        # Can be a playlist or a list of videos
        entries = yt_result['entries']

        #loops entries to grab each video_url
        for i, item in enumerate(video):
            results[i] = yt_result['entries'][i]

    return json.dumps(results)
