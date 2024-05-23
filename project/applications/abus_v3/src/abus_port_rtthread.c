#include "abus.h"
#include "rtdbg.h"
/* Compiler Related Definitions */
#include <rtthread.h>
/**
 * 将abus的命令对接到rtthread的msh

*/

//需要完成
//topic 信息显示
//列举所有topic
//列举所有acc

//
enum
{
    ABUS_OPT_TOPIC = 1U,
    ABUS_OPT_ACC,
    ABUS_OPT_ECHO,
};
CMD_OPTIONS_STATEMENT(cmd_abus_list)
static int cmd_abus_list(int argc, char **argv)
{
    if (argc >= 2)
    {
        switch (MSH_OPT_ID_GET(cmd_abus_list))
        {
        case ABUS_OPT_TOPIC:
            //list_all_motor();
            break;
        case ABUS_OPT_ACC:
            //set_get_torque(argc, argv);
            break;
        case ABUS_OPT_ECHO:
            //set_get_speed(argc, argv);
            break;
        default:
            goto _usage;
        };
        return 0;
    }

_usage:
    LOG_RAW("Usage: m [options]\n");
    LOG_RAW("[options]:\n");
    MSH_OPT_DUMP(cmd_abus_list);
    return 0;
}

CMD_OPTIONS_NODE_START(cmd_abus_list)
CMD_OPTIONS_NODE(ABUS_OPT_TOPIC, topic, list all motor id)
CMD_OPTIONS_NODE(ABUS_OPT_ACC, acc, set or get torque)
CMD_OPTIONS_NODE(ABUS_OPT_ECHO, echo, set or get speed)
CMD_OPTIONS_NODE_END

    MSH_CMD_EXPORT_ALIAS(cmd_abus_list, abus, abus operate cmd, optenable);
