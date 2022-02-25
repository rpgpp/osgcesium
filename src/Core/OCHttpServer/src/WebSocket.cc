#include "WebSocket.h"

using namespace OC;

struct Subscriber
{
    std::string chatRoomName_;
    drogon::SubscriberID id_;
};
void WebSocketTest::handleNewMessage(const WebSocketConnectionPtr &wsConnPtr,
                                     std::string &&message,
                                     const WebSocketMessageType &type)
{
    // write your application logic here
    LOG_INFO << "new websocket message:" << message;
    if (type == WebSocketMessageType::Ping)
    {
        LOG_INFO << "recv a ping";
    }
    else if (type == WebSocketMessageType::Text)
    {
        auto &s = wsConnPtr->getContextRef<Subscriber>();
        chatRooms_.publish(s.chatRoomName_, message);
    }
}

void WebSocketTest::handleConnectionClosed(const WebSocketConnectionPtr &conn)
{
    LOG_INFO << "websocket closed!";
    auto &s = conn->getContextRef<Subscriber>();
    chatRooms_.unsubscribe(s.chatRoomName_, s.id_);
}

void WebSocketTest::handleNewConnection(const HttpRequestPtr &req,
                                        const WebSocketConnectionPtr &conn)
{
    LOG_INFO << "new websocket connection!";
    conn->send("haha!!!");
    Subscriber s;
    s.chatRoomName_ = req->getParameter("room_name");
    s.id_ = chatRooms_.subscribe(s.chatRoomName_,
                                 [conn](const std::string &topic,
                                        const std::string &message) {
                                     conn->send(message);
                                 });
    conn->setContext(std::make_shared<Subscriber>(std::move(s)));
}
