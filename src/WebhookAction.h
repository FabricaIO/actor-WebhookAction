/*
* This file and associated .cpp file are licensed under the GPLv3 License Copyright (c) 2025 Sam Groveman
* 
* External libraries needed:
* ArduinoJSON: https://arduinojson.org/
*
* Contributors: Sam Groveman
*/
#pragma once
#include <Actor.h>
#include <Webhook.h>
#include <TimeInterface.h>

/// @brief Class describing a button that triggers a webhook
class WebhookAction : public Actor {
	public:
		WebhookAction(String Name, String url, std::map<String, String> customHeaders = {}, String configFile = "Hook.json");
		bool begin();
		std::tuple<bool, String> receiveAction(int action, String payload = "");
		String getConfig();
		bool setConfig(String config, bool save);

	protected:
		/// @brief Holds button webhook configuration
		struct {
			/// @brief The request method to use
			String method;
		 } hook_config;

		/// @brief Path to configuration file
		String config_path;

		/// @brief Webhook to for sending signal
		Webhook webhook;

		String callHook(String content);
};