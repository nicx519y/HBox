#include "configs/webconfig.hpp"
// #include "config.pb.h"
// #include "configs/base64.hpp"
#include "configmanager.hpp"
// #include <cstring>
#include <string>
// #include <vector>
// #include <memory>
// #include <set>
#include "cJSON.h"

// HTTPD Includes
#include "rndis.h"
#include "fs.h"
#include "fscustom.h"
#include "fsdata.h"
#include "lwip/apps/httpd.h"
#include "lwip/def.h"
#include "lwip/mem.h"
// #include "addons/input_macro.h"
// #include "bitmaps.hpp"
#include "constant.hpp"
#include "config.hpp"
#include "storagemanager.hpp"


#define PATH_CGI_ACTION "/cgi/action"

#define LWIP_HTTPD_POST_MAX_PAYLOAD_LEN (1024 * 16)

#define LWIP_HTTPD_RESPONSE_MAX_PAYLOAD_LEN (1024 * 16)

using namespace std;

// extern struct fsdata_file file__index_html[];

const static char* spaPaths[] = { "/backup", "/display-config", "/led-config", "/pin-mapping", "/settings", "/reset-settings", "/add-ons", "/custom-theme", "/macro", "/peripheral-mapping" };
const static char* excludePaths[] = { "/css", "/images", "/js", "/static" };
const static uint32_t rebootDelayMs = 500;
static string http_post_uri;
__RAM_Area__ static char http_post_payload[LWIP_HTTPD_POST_MAX_PAYLOAD_LEN];
static uint16_t http_post_payload_len = 0;
__RAM_Area__ static char http_response[LWIP_HTTPD_RESPONSE_MAX_PAYLOAD_LEN];
// static absolute_time_t rebootDelayTimeout = nil_time;
// static System::BootMode rebootMode = System::BootMode::DEFAULT;

void WebConfig::setup() {
    rndis_init();
}

void WebConfig::loop() {
    // rndis http server requires inline functions (non-class)
    rndis_task();

    // if (!is_nil_time(rebootDelayTimeout) && time_reached(rebootDelayTimeout)) {
    //     System::reboot(rebootMode);
    // }
}

enum class HttpStatusCode
{
    _200,
    _400,
    _500,
};

struct DataAndStatusCode
{
    DataAndStatusCode(string&& data, HttpStatusCode statusCode) :
        data(std::move(data)),
        statusCode(statusCode)
    {}

    string data;
    HttpStatusCode statusCode;
};

// **** WEB SERVER Overrides and Special Functionality ****
int set_file_data(fs_file* file, const DataAndStatusCode& dataAndStatusCode)
{
    static string returnData;

    const char* statusCodeStr = "";
    switch (dataAndStatusCode.statusCode)
    {
        case HttpStatusCode::_200: statusCodeStr = "200 OK"; break;
        case HttpStatusCode::_400: statusCodeStr = "400 Bad Request"; break;
        case HttpStatusCode::_500: statusCodeStr = "500 Internal Server Error"; break;
    }

    returnData.clear();
    returnData.append("HTTP/1.0 ");
    returnData.append(statusCodeStr);
    returnData.append("\r\n");
    returnData.append(
        "Server: GP2040-CE \r\n"
        "Content-Type: application/json\r\n"
        "Access-Control-Allow-Origin: *\r\n"
        "Content-Length: "
    );
    returnData.append(std::to_string(dataAndStatusCode.data.length()));
    returnData.append("\r\n\r\n");
    returnData.append(dataAndStatusCode.data);

    file->data = returnData.c_str();
    file->len = returnData.size();
    file->index = file->len;
    file->http_header_included = file->http_header_included;
    file->pextension = NULL;

    return 1;
}

int set_file_data(fs_file *file, string&& data)
{
    if (data.empty())
        return 0;
    return set_file_data(file, DataAndStatusCode(std::move(data), HttpStatusCode::_200));
}

cJSON* get_post_data()
{
    return cJSON_Parse(http_post_payload);                                                                            
}

// LWIP callback on HTTP POST to validate the URI
err_t httpd_post_begin(void *connection, const char *uri, const char *http_request,
                       uint16_t http_request_len, int content_len, char *response_uri,
                       uint16_t response_uri_len, uint8_t *post_auto_wnd)
{
    LWIP_UNUSED_ARG(http_request);
    LWIP_UNUSED_ARG(http_request_len);
    LWIP_UNUSED_ARG(content_len);
    LWIP_UNUSED_ARG(response_uri);
    LWIP_UNUSED_ARG(response_uri_len);
    LWIP_UNUSED_ARG(post_auto_wnd);

    if (!uri || strncmp(uri, "/api", 4) != 0) {
        return ERR_ARG;
    }

    http_post_uri = uri;
    http_post_payload_len = 0;
    memset(http_post_payload, 0, LWIP_HTTPD_POST_MAX_PAYLOAD_LEN);
    return ERR_OK;
}

// LWIP callback on HTTP POST to for receiving payload
err_t httpd_post_receive_data(void *connection, struct pbuf *p)
{
    LWIP_UNUSED_ARG(connection);

    // Cache the received data to http_post_payload
    while (p != NULL)
    {
        if (http_post_payload_len + p->len <= LWIP_HTTPD_POST_MAX_PAYLOAD_LEN)
        {
            MEMCPY(http_post_payload + http_post_payload_len, p->payload, p->len);
            http_post_payload_len += p->len;
        }
        else // Buffer overflow
        {
            http_post_payload_len = 0xffff;
            break;
        }

        p = p->next;
    }

    // Need to release memory here or will leak
    pbuf_free(p);

    // If the buffer overflows, error out
    if (http_post_payload_len == 0xffff) {
        return ERR_BUF;
    }

    return ERR_OK;
}

// LWIP callback to set the HTTP POST response_uri, which can then be looked up via the fs_custom callbacks
void httpd_post_finished(void *connection, char *response_uri, uint16_t response_uri_len)
{
    LWIP_UNUSED_ARG(connection);

    if (http_post_payload_len != 0xffff) {
        strncpy(response_uri, http_post_uri.c_str(), response_uri_len);
        response_uri[response_uri_len - 1] = '\0';
    }
}


/* ======================================= apis begin ==================================================== */

std::string get_response_temp(STORAGE_ERROR_NO errNo, cJSON* data)
{
    cJSON* json = cJSON_CreateObject();
    cJSON_AddNumberToObject(json, "errNo", errNo);
    cJSON_AddItemToObject(json, "data", data!=NULL?data:cJSON_CreateObject());
    memset(http_response, '\0', sizeof(http_response));
    cJSON_PrintBuffered(json, *http_response, 0);
    cJSON_Delete(json);
    return http_response;
}

/**
 * @brief 获取固件版本号
 * 
 * @return std::string
 * {
 *      "errNo": 0,
 *      "data": { "firmwareVersion": 0x100000 }
 * }
 */
std::string getFirmwareVersion()
{
    cJSON* dataJSON = cJSON_CreateObject();
    cJSON_AddNumberToObject(dataJSON, "firmwareVersion", FIRMWARE_VERSION);
    std::string result = get_response_temp(STORAGE_ERROR_NO::ACTION_SUCCESS, dataJSON);
    cJSON_Delete(dataJSON);
    return result;
}

/**
 * @brief 获取当前生效的profile索引
 * 
 * @return std::string 
 * {
 *      "errNo": 0,
 *      "data": { "profileIndex": 0 }
 * }
 */
std::string getActiveProfileIndex()
{
    Config* config = &Storage::getInstance().config;
    cJSON* dataJSON = cJSON_CreateObject();
    cJSON_AddNumberToObject(dataJSON, "profileIndex", config->profileIndex);
    std::string result = get_response_temp(STORAGE_ERROR_NO::ACTION_SUCCESS, dataJSON);
    cJSON_Delete(dataJSON);
    return result;
}

/**
 * @brief 获取当前profile的KeyMapping
 * 
 * @return std::string 
 * {
 *      "errNo": 0,
 *      "data": {
 *          "profileIndex": 0,
 *          "inputMode": 0,
 *          "invertXAxis": false,
 *          "invertYAxis": false,
 *          "fourWayMode": false,
 *          "keyDpadUp": 0x0000000100000000,
 *          ...
 *      }
 * }
 */
std::string getKeyMappings()
{
    Config* config = &Storage::getInstance().config;
    GamepadOptions* profile = config->profiles[config->profileIndex];
    cJSON* dataJSON = cJSON_CreateObject();

    cJSON_AddNumberToObject(dataJSON, "profileIndex", config->profileIndex);

    cJSON_AddNumberToObject(dataJSON, "inputMode", profile->inputMode);
    cJSON_AddNumberToObject(dataJSON, "socdMode", profile->socdMode);
    cJSON_AddBoolToObject(dataJSON, "invertXAxis", profile->invertXAxis?1:0);
    cJSON_AddBoolToObject(dataJSON, "invertYAxis", profile->invertYAxis?1:0);
    cJSON_AddBoolToObject(dataJSON, "fourWayMode", profile->fourWayMode?1:0);

    cJSON_AddNumberToObject(dataJSON, "keyDpadUp", profile->keyDpadUp);
    cJSON_AddNumberToObject(dataJSON, "keyDpadDown", profile->keyDpadDown);
    cJSON_AddNumberToObject(dataJSON, "keyDpadLeft", profile->keyDpadLeft);
    cJSON_AddNumberToObject(dataJSON, "keyDpadRight", profile->keyDpadRight);
    cJSON_AddNumberToObject(dataJSON, "keyButtonB1", profile->keyButtonB1);
    cJSON_AddNumberToObject(dataJSON, "keyButtonB2", profile->keyButtonB2);
    cJSON_AddNumberToObject(dataJSON, "keyButtonB3", profile->keyButtonB3);
    cJSON_AddNumberToObject(dataJSON, "keyButtonB4", profile->keyButtonB4);
    cJSON_AddNumberToObject(dataJSON, "keyButtonL1", profile->keyButtonL1);
    cJSON_AddNumberToObject(dataJSON, "keyButtonR1", profile->keyButtonR1);
    cJSON_AddNumberToObject(dataJSON, "keyButtonL2", profile->keyButtonL2);
    cJSON_AddNumberToObject(dataJSON, "keyButtonR2", profile->keyButtonR2);
    cJSON_AddNumberToObject(dataJSON, "keyButtonS1", profile->keyButtonS1);
    cJSON_AddNumberToObject(dataJSON, "keyButtonS2", profile->keyButtonS2);
    cJSON_AddNumberToObject(dataJSON, "keyButtonL3", profile->keyButtonL3);
    cJSON_AddNumberToObject(dataJSON, "keyButtonR3", profile->keyButtonR3);
    cJSON_AddNumberToObject(dataJSON, "keyButtonA1", profile->keyButtonA1);
    cJSON_AddNumberToObject(dataJSON, "keyButtonA2", profile->keyButtonA2);
    cJSON_AddNumberToObject(dataJSON, "keyButtonFn", profile->keyButtonFn);

    std::string result = get_response_temp(STORAGE_ERROR_NO::ACTION_SUCCESS, dataJSON);
    cJSON_Delete(dataJSON);
    return result;
}

/**
 * @brief 获取当前profile的LEDSOptions
 * 
 * @return std::string 
 * {
 *      "errNo": 0,
 *      "data": {
 *          "profileIndex": 0,
 *          "ledEnabled": true,
 *          "ledEffect": 0,
 *          "ledColor1": 0xff0000,
 *          ...
 *      }
 * }
 */
std::string getLEDsOptions()
{
    Config* config = &Storage::getInstance().config;
    GamepadOptions* profile = config->profiles[config->profileIndex];
    cJSON* dataJSON = cJSON_CreateObject();

    cJSON_AddNumberToObject(dataJSON, "profileIndex", config->profileIndex);

    cJSON_AddBoolToObject(dataJSON, "ledEnabled", profile->ledEnabled?1:0);
    cJSON_AddNumberToObject(dataJSON, "ledEffect", profile->ledEffect);
    cJSON_AddBoolToObject(dataJSON, "isDoubleColor", profile->isDoubleColor?1:0);
    cJSON_AddNumberToObject(dataJSON, "ledColor1", profile->ledColor1);
    cJSON_AddNumberToObject(dataJSON, "ledColor2", profile->ledColor2);
    cJSON_AddNumberToObject(dataJSON, "ledColor3", profile->ledColor3);
    cJSON_AddNumberToObject(dataJSON, "ledBrightness", profile->ledBrightness);
    cJSON_AddNumberToObject(dataJSON, "ledColorCalibrateTop", profile->ledColorCalibrateTop);
    cJSON_AddNumberToObject(dataJSON, "ledColorCalibrateBottom", profile->ledColorCalibrateBottom);
    cJSON_AddNumberToObject(dataJSON, "ledColorCalibrateComplete", profile->ledColorCalibrateComplete);
    cJSON_AddNumberToObject(dataJSON, "ledBrightnesssCalibrate", profile->ledBrightnesssCalibrate);

    std::string result = get_response_temp(STORAGE_ERROR_NO::ACTION_SUCCESS, dataJSON);
    cJSON_Delete(dataJSON);
    return result;
}

/**
 * @brief 获取ADC按钮列表
 * 
 * @return std::string 
 * {
 *      "errNo": 0,
 *      "data": [
 *          {
 *              "virtualPin": 0,
 *              "pressAccuracy": 0,
 *              ...
 *          },
 *          ...
 *      ]
 * }
 */
std::string getADCButtons()
{
    Config* config = &Storage::getInstance().config;
    ADCButton** configBtns = config->ADCButtons;
    cJSON* dataJSON = cJSON_CreateArray();

    uint8_t len = sizeof(configBtns) / sizeof(configBtns[0]);

    for(uint8_t i = 0; i < len; i ++) {
        cJSON* btn = cJSON_CreateObject();
        cJSON_AddItemToArray(dataJSON, btn);

        cJSON_AddNumberToObject(btn, "virtualPin", configBtns[i]->virtualPin);
        cJSON_AddNumberToObject(btn, "magnettization", configBtns[i]->magnettization);
        cJSON_AddNumberToObject(btn, "topPosition", configBtns[i]->topPosition);
        cJSON_AddNumberToObject(btn, "bottomPosition", configBtns[i]->bottomPosition);
    }

    std::string result = get_response_temp(STORAGE_ERROR_NO::ACTION_SUCCESS, dataJSON);
    cJSON_Delete(dataJSON);
    return result;
}

/**
 * @brief 获取GPIO按钮列表
 * 
 * @return std::string 
 * {
 *      "errNo": 0,
 *      "data": [
 *          {
 *              "virtualPin": 0,
 *          },
 *          ...
 *      ]
 * }
 */
std::string getGPIOButtons()
{
    Config* config = &Storage::getInstance().config;
    GPIOButton** configBtns = config->GPIOButtons;
    cJSON* dataJSON = cJSON_CreateArray();

    uint8_t len = sizeof(configBtns) / sizeof(configBtns[0]);

    for(uint8_t i = 0; i < len; i ++) {
        cJSON* btn = cJSON_CreateObject();
        cJSON_AddItemToArray(dataJSON, btn);
        cJSON_AddNumberToObject(btn, "virtualPin", configBtns[i]->virtualPin);
    }

    std::string result = get_response_temp(STORAGE_ERROR_NO::ACTION_SUCCESS, dataJSON);
    cJSON_Delete(dataJSON);
    return result;
}

/**
 * @brief 重启
 * 
 * @return std::string 
 * {
 *      "errNo": 0,
 *      "data": {}
 * }
 */
std::string systemReboot()
{
    NVIC_SystemReset();
    return get_response_temp(STORAGE_ERROR_NO::ACTION_SUCCESS, NULL);
}

/**
 * @brief 重置设置
 * 
 * @return std::string 
 * {
 *      "errNo": 0,
 *      "data": {}
 * }
 */
std::string resetSetting()
{
    if(Storage::getInstance().ResetSettings()) {
        return get_response_temp(STORAGE_ERROR_NO::ACTION_SUCCESS, NULL);
    } else {
        return get_response_temp(STORAGE_ERROR_NO::ACTION_FAILURE, NULL);
    }
}

/**
 * @brief 设置当前生效的profile index
 * 
 * @return std::string 
 * {
 *      "errNo": 0,
 *      "data": {}
 * }
 */
std::string setActiveProfileIndex()
{
    cJSON* response = get_post_data();

    if(response == NULL) {
        cJSON_Delete(response);
        return get_response_temp(STORAGE_ERROR_NO::PARAMETERS_ERROR, NULL);
    }

    uint8_t profileIndex = cJSON_GetObjectItem(response, "profileIndex")->valueint & 0xff;

    if(profileIndex == NULL || profileIndex < 0 || profileIndex > NUM_PROFILES) {
        cJSON_Delete(response);
        return get_response_temp(STORAGE_ERROR_NO::PARAMETERS_ERROR, NULL);
    }

    cJSON_Delete(response);
    Config* config = &Storage::getInstance().config;
    config->profileIndex = profileIndex;

    if(Storage::getInstance().save()) {
        return get_response_temp(STORAGE_ERROR_NO::ACTION_SUCCESS, NULL);
    } else {
        return get_response_temp(STORAGE_ERROR_NO::ACTION_FAILURE, NULL);
    }
}

/**
 * @brief 设置当前profile的keymapping
 * 
 * @return std::string 
 */
std::string setKeyMappings()
{
    cJSON* response = get_post_data();
    
    if(response == NULL) {
        cJSON_Delete(response);
        return get_response_temp(STORAGE_ERROR_NO::PARAMETERS_ERROR, NULL);
    }

    uint8_t profileIndex = cJSON_GetObjectItem(response, "profileIndex")->valueint & 0xff;

    if(profileIndex == NULL || profileIndex < 0 || profileIndex > NUM_PROFILES) {
        cJSON_Delete(response);
        return get_response_temp(STORAGE_ERROR_NO::PARAMETERS_ERROR, NULL);
    }

    char* intKeys[] = {
        "inputMode", "socdMode", "keyDpadUp", "keyDpadDown", "keyDpadLeft", "keyDpadRight", "keyButtonB1",
        "keyButtonB2", "keyButtonB3", "keyButtonB4", "keyButtonL1", "keyButtonR1", "keyButtonL2", "keyButtonR2",
        "keyButtonS1", "keyButtonS2", "keyButtonL3", "keyButtonR3", "keyButtonA1", "keyButtonA2", "keyButtonFn"
    };

    uint8_t lenIntKey = sizeof(intKeys) / sizeof(intKeys[0]);
    for(uint8_t i = 0; i < lenIntKey; i ++) {
        if(cJSON_GetObjectItem(response, intKeys[i])->type != cJSON_Number) {
            cJSON_Delete(response);
            return get_response_temp(STORAGE_ERROR_NO::PARAMETERS_ERROR, NULL);
        }
    }

    GamepadOptions* profile = Storage::getInstance().config.profiles[profileIndex];
    profile->inputMode = (InputMode) cJSON_GetObjectItem(response, "inputMode")->valueint;
    profile->socdMode = (SOCDMode) cJSON_GetObjectItem(response, "socdMode")->valueint;
    profile->invertXAxis = cJSON_GetObjectItem(response, "invertXAxis")->type == cJSON_True? true: false;
    profile->invertYAxis = cJSON_GetObjectItem(response, "invertYAxis")->type == cJSON_True? true: false;
    profile->fourWayMode = cJSON_GetObjectItem(response, "fourWayMode")->type == cJSON_True? true: false;

    profile->keyDpadUp = (uint32_t) cJSON_GetObjectItem(response, "keyDpadUp")->valueint;
    profile->keyDpadDown = (uint32_t) cJSON_GetObjectItem(response, "keyDpadDown")->valueint;
    profile->keyDpadLeft = (uint32_t) cJSON_GetObjectItem(response, "keyDpadLeft")->valueint;
    profile->keyDpadRight = (uint32_t) cJSON_GetObjectItem(response, "keyDpadRight")->valueint;
    profile->keyButtonB1 = (uint32_t) cJSON_GetObjectItem(response, "keyButtonB1")->valueint;
    profile->keyButtonB2 = (uint32_t) cJSON_GetObjectItem(response, "keyButtonB2")->valueint;
    profile->keyButtonB3 = (uint32_t) cJSON_GetObjectItem(response, "keyButtonB3")->valueint;
    profile->keyButtonB4 = (uint32_t) cJSON_GetObjectItem(response, "keyButtonB4")->valueint;
    profile->keyButtonL1 = (uint32_t) cJSON_GetObjectItem(response, "keyButtonL1")->valueint;
    profile->keyButtonR1 = (uint32_t) cJSON_GetObjectItem(response, "keyButtonR1")->valueint;
    profile->keyButtonL2 = (uint32_t) cJSON_GetObjectItem(response, "keyButtonL2")->valueint;
    profile->keyButtonR2 = (uint32_t) cJSON_GetObjectItem(response, "keyButtonR2")->valueint;
    profile->keyButtonS1 = (uint32_t) cJSON_GetObjectItem(response, "keyButtonS1")->valueint;
    profile->keyButtonS2 = (uint32_t) cJSON_GetObjectItem(response, "keyButtonS2")->valueint;
    profile->keyButtonL3 = (uint32_t) cJSON_GetObjectItem(response, "keyButtonL3")->valueint;
    profile->keyButtonR3 = (uint32_t) cJSON_GetObjectItem(response, "keyButtonR3")->valueint;
    profile->keyButtonA1 = (uint32_t) cJSON_GetObjectItem(response, "keyButtonA1")->valueint;
    profile->keyButtonA2 = (uint32_t) cJSON_GetObjectItem(response, "keyButtonA2")->valueint;
    profile->keyButtonFn = (uint32_t) cJSON_GetObjectItem(response, "keyButtonFn")->valueint;

    cJSON_Delete(response);

    if(Storage::getInstance().save()) {
        return get_response_temp(STORAGE_ERROR_NO::ACTION_SUCCESS, NULL);
    } else {
        return get_response_temp(STORAGE_ERROR_NO::ACTION_FAILURE, NULL);
    }
}

/**
 * @brief 设置当前profile的LED options
 * 
 * @return std::string 
 */
std::string setLEDsOptions()
{
    cJSON* response = get_post_data();
    
    if(response == NULL) {
        cJSON_Delete(response);
        return get_response_temp(STORAGE_ERROR_NO::PARAMETERS_ERROR, NULL);
    }

    uint8_t profileIndex = cJSON_GetObjectItem(response, "profileIndex")->valueint & 0xff;

    if(profileIndex == NULL || profileIndex < 0 || profileIndex > NUM_PROFILES) {
        cJSON_Delete(response);
        return get_response_temp(STORAGE_ERROR_NO::PARAMETERS_ERROR, NULL);
    }

    char* intKeys[] = {
        "ledEffect", "ledColor1", "ledColor2", "ledColor3", "ledBrightness", 
        "ledColorCalibrateTop", "ledColorCalibrateBottom",
        "ledColorCalibrateComplete", "ledBrightnesssCalibrate"
    };

    uint8_t lenIntKey = sizeof(intKeys) / sizeof(intKeys[0]);
    for(uint8_t i = 0; i < lenIntKey; i ++) {
        if(cJSON_GetObjectItem(response, intKeys[i])->type != cJSON_Number) {
            cJSON_Delete(response);
            return get_response_temp(STORAGE_ERROR_NO::PARAMETERS_ERROR, NULL);
        }
    }

    GamepadOptions* profile = Storage::getInstance().config.profiles[profileIndex];
    profile->ledEnabled = cJSON_GetObjectItem(response, "keyDpadUp")->type == cJSON_True? true: false;
    profile->isDoubleColor = cJSON_GetObjectItem(response, "isDoubleColor")->type == cJSON_True? true: false;
    profile->ledEffect = (LEDEffect) cJSON_GetObjectItem(response, "ledEffect")->valueint;
    profile->ledColor1 = (uint32_t) cJSON_GetObjectItem(response, "ledColor1")->valueint;
    profile->ledColor2 = (uint32_t) cJSON_GetObjectItem(response, "ledColor2")->valueint;
    profile->ledColor3 = (uint32_t) cJSON_GetObjectItem(response, "ledColor3")->valueint;
    profile->ledBrightness = (uint8_t) cJSON_GetObjectItem(response, "ledBrightness")->valueint;
    profile->ledColorCalibrateTop = (uint32_t) cJSON_GetObjectItem(response, "ledColorCalibrateTop")->valueint;
    profile->ledColorCalibrateBottom = (uint32_t) cJSON_GetObjectItem(response, "ledColorCalibrateBottom")->valueint;
    profile->ledColorCalibrateComplete = (uint32_t) cJSON_GetObjectItem(response, "ledColorCalibrateComplete")->valueint;
    profile->ledBrightnesssCalibrate = (uint32_t) cJSON_GetObjectItem(response, "ledBrightnesssCalibrate")->valueint;

    cJSON_Delete(response);

    if(Storage::getInstance().save()) {
        return get_response_temp(STORAGE_ERROR_NO::ACTION_SUCCESS, NULL);
    } else {
        return get_response_temp(STORAGE_ERROR_NO::ACTION_FAILURE, NULL);
    }
}

/**
 * @brief 测试用
 * 
 * @return std::string 
 */
std::string echo()
{
    cJSON* response = get_post_data();
    std::string result = get_response_temp(STORAGE_ERROR_NO::ACTION_SUCCESS, response);
    cJSON_Delete(response);
    return result;
}

/*============================================ apis end =================================================*/


typedef std::string (*HandlerFuncPtr)();
static const std::pair<const char*, HandlerFuncPtr> handlerFuncs[] =
{
    { "/api/getFirmwareVersion",  getFirmwareVersion },
    { "/api/getActiveProfileIndex",  getActiveProfileIndex },
    { "/api/getPinMappings", getKeyMappings },
    { "/api/getLEDsOptions", getLEDsOptions },
    { "/api/getADCButtons", getADCButtons },
    { "/api/getGPIOButtons", getGPIOButtons },
    { "/api/systemReboot", systemReboot },
    { "/api/resetSetting", resetSetting },
    { "/api/setActiveProfileIndex", setActiveProfileIndex },
    { "/api/setPinMappings", setKeyMappings },
    { "/api/setLEDsOptions", setLEDsOptions },
#if !defined(NDEBUG)
    // { "/api/echo", echo },
#endif
};

typedef DataAndStatusCode (*HandlerFuncStatusCodePtr)();
static const std::pair<const char*, HandlerFuncStatusCodePtr> handlerFuncsWithStatusCode[] =
{
    // { "/api/setConfig", setConfig },
};


int fs_open_custom(struct fs_file *file, const char *name)
{
    for (const auto& handlerFunc : handlerFuncs)
    {
        if (strcmp(handlerFunc.first, name) == 0)
        {
            return set_file_data(file, handlerFunc.second());
        }
    }

    for (const auto& handlerFunc : handlerFuncsWithStatusCode)
    {
        if (strcmp(handlerFunc.first, name) == 0)
        {
            return set_file_data(file, handlerFunc.second());
        }
    }

    for (const char* excludePath : excludePaths)
        if (strcmp(excludePath, name) == 0)
            return 0;

    for (const char* spaPath : spaPaths)
    {
        if (strcmp(spaPath, name) == 0)
        {
            file->data = (const char *)file__index_html[0].data;
            file->len = file__index_html[0].len;
            file->index = file__index_html[0].len;
            file->http_header_included = file__index_html[0].http_header_included;
            file->pextension = NULL;
            file->is_custom_file = 0;
            return 1;
        }
    }

    return 0;
}

void fs_close_custom(struct fs_file *file)
{
    if (file && file->is_custom_file && file->pextension)
    {
        mem_free(file->pextension);
        file->pextension = NULL;
    }
}
