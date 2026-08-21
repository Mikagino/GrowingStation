#pragma once

// This file will contain all the secrets but is commited once to be available everytime
class Secrets
{
public:
    struct TelegramBot
    {
        const char *TOKEN = "";
        const char *CHAT_ID = "";
    };

    struct WiFi
    {
        const char *SSID = "";
        const char *PASSWORD = "";
    }
};