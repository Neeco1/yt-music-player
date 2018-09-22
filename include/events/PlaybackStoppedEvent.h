#ifndef PLAYBACK_STOPPED_EVENT_H__
#define PLAYBACK_STOPPED_EVENT_H__

class PlaybackStoppedEvent : public Event
{
public:
    PlaybackStoppedEvent(std::string const & msg) :
    Event(nullptr),
    msg(msg) {
    }

    virtual ~PlaybackStoppedEvent() { }

    std::string const & getMessage() {
        return msg;
    }

private:
    std::string const & msg;
    
};

#endif
