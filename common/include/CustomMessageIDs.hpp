#ifndef _CUSTOM_MESSAGES_
#define _CUSTOM_MESSAGES_
#include <cstdint>

namespace RemoteShell {

    enum TCPCustomMessageID : uint32_t {
        SERVER_ACCEPT,              //Body : 
        SERVER_DISCONNECTED,        //Body : 
        CLIENT_DISCONNECTED,        //Body : 
        SHELL_CMD,                  //Body : char[MAX_CMD_LENGTH] cmd;
        SHELL_OUTPUT,               //Body : char[MAX_OUTPUT_LENGTH] output;
    };

    enum UDPCustomMessageID : uint32_t {
        CLIENT_CONNECTED,           //Body : 
        CLIENT_DISCONNECTED_UDP,    //Body : 
        SERVER_CLIENT_DISCONNECTED, //Body : 
        LOAD_PREFAB,                //Body : char[PREFAB_MAX_LENGTH] name;
        CONSTRUCT_ENTITY,           //Body : char[PREFAB_MAX_LENGTH] name;
        COUNTDOWN,                  //Body : uint32_t countdown;
        NBPLAYERS,                  //Body : uint32_t nbPlayers;
        GAME_START,                 //Body : 
        GAME_FINISH,                //Body : 
        SET_ID_PLAYER,              //Body : uint32_t id;
        UPDATE_PLAYER_CONTROLLER,   //Body : uint32_t PlayerUpdate //Juste la direction
        UPDATE_COMPONENT_COLLIDER,  //Body : uint32 (entity) << ColliderComponent
        UPDATE_COMPONENT_CONTROL,   //Body : uint32 (entity) << ControllerComponent
        UPDATE_COMPONENT_OVER,      //Body : uint32 (entity) << isOverComponent
        UPDATE_COMPONENT_POSITION,  //Body : uint32 (entity) << PositionComponent
        UPDATE_COMPONENT_SPEED,     //Body : uint32 (entity) << SpeedComponent
        UPDATE_COMPONENT_STAT,      //Body : uint32 (entity) << unsigned int (lp) << unsigned int (invinsibility)
        UPDATE_COMPONENT_WAVE,      //Body : uint32 (entity) << WaveComponent
        UPDATE_COMPONENT_WEAPON,    //Body : uint32 (entity) << int
    };
};

#endif