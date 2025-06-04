#include"WebhookAction.h"
/// @brief Creates a webhook
/// @param Name The device name
/// @param Url The URL to call
/// @param customHeaders a <String, String> map of custom headers
/// @param configFile Name of the config file to use
WebhookAction::WebhookAction(String Name, String Url, std::map<String, String> customHeaders, String configFile) : webhook(Url, customHeaders), Actor(Name) {
	config_path = "/settings/act/" + configFile;
}

/// @brief Starts a webhook 
/// @return True on success
bool WebhookAction::begin() {
	// Set description
	Description.actionQuantity = 1;
	Description.type = "webhook";
	Description.actions = {{"Fire Webhook", 0}};
	// Create settings if necessary
	if (!checkConfig(config_path)) {
		// Set defaults
		hook_config.method = "GET";
		return setConfig(getConfig(), true);
	} else {
		// Load settings
		return setConfig(Storage::readFile(config_path), false);
	}
}

/// @brief Receives an action
/// @param action The action to process (only option is 0 for fire webhook)
/// @param payload The content body to send
/// @return JSON response with OK
std::tuple<bool, String> WebhookAction::receiveAction(int action, String payload) {
	String response = R"({"success": false, "Response": "bad command"})";
	if (action == 0) {
		response = callHook(payload);
	}	
	return {true,  response};
}

/// @brief Gets the current config
/// @return A JSON string of the config
String WebhookAction::getConfig() {
	// Allocate the JSON document
  	JsonDocument doc;
	doc["method"]["current"] = hook_config.method;
	doc["method"]["options"][0] = "POST";
	doc["method"]["options"][1] = "GET";
	doc["url"] = webhook.webhook_config.url;

	// Add name
	doc["Name"] = Description.name;

	// Create string to hold output
	String output;
	
	// Serialize to string
	serializeJson(doc, output);
	return output;
}

/// @brief Sets the configuration for this device
/// @param config A JSON string of the configuration settings
/// @param save If the configuration should be saved to a file
/// @return True on success
bool WebhookAction::setConfig(String config, bool save) {
	// Allocate the JSON document
	JsonDocument doc;
	// Deserialize file contents
	DeserializationError error = deserializeJson(doc, config);
	// Test if parsing succeeds.
	if (error) {
		Logger.print(F("Deserialization failed: "));
		Logger.println(error.f_str());
		return false;
	}
	// Assign loaded values
	Description.name = doc["Name"].as<String>();
	webhook.webhook_config.url = doc["url"].as<String>();
	hook_config.method = doc["method"]["current"].as<String>();
	
	// Save config if requested
	if (save) {
		return saveConfig(config_path, config);
	}
	return true;
}

/// @brief Calls the webhook
/// @param content The content of the request
/// @return Any response string
String WebhookAction::callHook(String content) {	
	content.replace("%TIMESTAMP%", String(TimeInterface::getEpoch()));
	if (hook_config.method == "POST") {
		return webhook.postRequest(content);
	} else if (hook_config.method == "GET") {
		return webhook.getRequest(content);
	}
	return R"({"success": false, "Response": "bad request method"})";
}