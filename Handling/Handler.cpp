#include "Handler.h"


void SigContHandler(int)
{

}

PacketHandler::PacketHandler():
_diffTime(0), process(true)
{

}

PacketHandler::~PacketHandler()
{
    while (queue.size())
        queue.pop();
    delete sigHandler;
}

void PacketHandler::AddEvent(Event event)
{
    queue.push(event);
}

void PacketHandler::AddDelayedEvent(DelayedEvent event)
{
    delayedQueue.push_back(event);
}

void PacketHandler::UpdateDelayed()
{
    if (!delayedQueue.size())
        return;

    for (DelayedEventQueue::iterator it = delayedQueue.begin(); it != delayedQueue.end(); it++)
    {
        it->Update(getMsTimeDiffToNow(it->updated));
        if (it->Execute())
            queue.push(*it);
    }

    for (DelayedEventQueue::iterator it = delayedQueue.begin(); it != delayedQueue.end(); it++)
        if (it->Execute())
        {
            delayedQueue.erase(it);
            it = delayedQueue.begin();
            if (!delayedQueue.size())
                break;
        }
}

void PacketHandler::Terminate()
{
    sLog->outControl();
    sLog->outControl("[PacketHandler] Droping %d delayed events.", delayedQueue.size());
    delayedQueue.clear();
    sLog->outControl("[PacketHandler] Droping %d immediate events.", queue.size());
    while (queue.size())
        queue.pop();
    process = false;
}

void PacketHandler::ProcessQueue()
{
    assert(app);
    sigHandler = new SignalHandler();
    sigHandler->setSignalHandler(Continue, SigContHandler);
    sLog->outControl("[PacketHandler] Process Queue initialization");

    timeval workBegan;
    gettimeofday(&workBegan, 0);

    while (process)
    {
        UpdateDelayed();
        _diffTime = getMsTimeDiffToNow(workBegan);
        if (!queue.size() && !delayedQueue.size())
        {
            //sLog->outDebug("Process Queue goes to sleep (until data arrives)");
            app->threadMgr->SetThreadStatus(GetThisThread(), THREAD_SUSPENDED);
            app->freezeDetector->Pause();
            Thread::SuspendThisThread();
            app->threadMgr->SetThreadStatus(GetThisThread(), THREAD_ACTIVE);
            app->freezeDetector->Continue();
            //sLog->outDebug("Process Queue was suspended %lu milliseconds", getMsTimeDiffToNow(workBegan));
        }
        gettimeofday(&workBegan, 0);
        if (queue.size())
        {
            Event& event = queue.front();
            switch (event.EventType)
            {
                case EVENT_CONNECT:
                    if (app->proto.OnConnect)
                        app->proto.OnConnect(event.address, event.fd);
                    break;
                case EVENT_RECIEVE:
                    if (app->proto.OnRecieve)
                        app->proto.OnRecieve(event.address, event.fd, event.data);
                    delete &event.data;
                    break;
                case EVENT_SEND:
                    if (app->proto.OnSend)
                        app->proto.OnSend(event.data);
                    delete &event.data;
                    break;
                case EVENT_DISCONNECT:
                    if (app->proto.OnDisconnect)
                        app->proto.OnDisconnect(event.address, event.fd);
                    break;
                case EVENT_CONNECT_DELAYED:
                    if (app->proto.OnConnectDelayed)
                        app->proto.OnConnectDelayed(event.address, event.fd);
                    break;
                case EVENT_RECIEVE_DELAYED:
                    if (app->proto.OnRecieveDelayed)
                        app->proto.OnRecieveDelayed(event.address, event.fd, event.data);
                    delete &event.data;
                    break;
                case EVENT_SEND_DELAYED:
                    if (app->proto.OnSendDelayed)
                        app->proto.OnSendDelayed(event.data);
                    delete &event.data;
                    break;
                case EVENT_DISCONNECT_DELAYED:
                    if (app->proto.OnDisconnectDelayed)
                        app->proto.OnDisconnectDelayed(event.address, event.fd);
                    break;
                default:
                    sLog->outError("Unimplemented EventType:  %d", event.EventType);
            }
            //sLog->outDebug("Process Queue Update after diff: %lu milliseconds", getMsTimeDiffToNow(workBegan));
            queue.pop();
        }
        else
            if (delayedQueue.size())
                msleep(500);
    }
    app->freezeDetector->Exit();
    sLog->outControl("[PacketHandler] Process Queue exit");
    ThreadMgr::Exit(NULL);
}
