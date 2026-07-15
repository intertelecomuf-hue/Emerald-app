package com.emerald.app.ai

import com.google.gson.Gson
import com.google.gson.annotations.SerializedName
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.withContext
import okhttp3.MediaType.Companion.toMediaType
import okhttp3.OkHttpClient
import okhttp3.Request
import okhttp3.RequestBody.Companion.toRequestBody
import java.util.concurrent.TimeUnit

data class ChatMessage(
    val role: String, // "user" or "assistant"
    val content: String
)

// OpenAI API request/response models
private data class OpenAiRequest(
    val model: String,
    val messages: List<OpenAiMessage>,
    val temperature: Double = 0.7,
    val max_tokens: Int = 1000
)

private data class OpenAiMessage(
    val role: String,
    val content: String
)

private data class OpenAiResponse(
    val choices: List<OpenAiChoice>?,
    val error: OpenAiError?
)

private data class OpenAiChoice(
    val message: OpenAiMessage
)

private data class OpenAiError(
    val message: String
)

object AiService {
    private const val DEFAULT_API_URL = "https://api.openai.com/v1/chat/completions"
    private const val DEFAULT_MODEL = "gpt-3.5-turbo"

    // You can change these before calling sendMessage
    var apiUrl: String = DEFAULT_API_URL
    var apiKey: String = ""
    var model: String = DEFAULT_MODEL

    private val client = OkHttpClient.Builder()
        .connectTimeout(30, TimeUnit.SECONDS)
        .readTimeout(60, TimeUnit.SECONDS)
        .writeTimeout(30, TimeUnit.SECONDS)
        .build()

    private val gson = Gson()

    suspend fun sendMessage(
        messages: List<ChatMessage>,
        systemPrompt: String = "Ти — BotShe, дружній AI-помічник. Відповідай українською мовою."
    ): Result<String> {
        return withContext(Dispatchers.IO) {
            try {
                if (apiKey.isBlank()) {
                    return@withContext Result.failure(
                        Exception("API ключ не налаштовано. Налаштуйте його в розділі Акаунт.")
                    )
                }

                val openAiMessages = mutableListOf(
                    OpenAiMessage("system", systemPrompt)
                )
                openAiMessages.addAll(
                    messages.map { OpenAiMessage(it.role, it.content) }
                )

                val requestBody = OpenAiRequest(
                    model = model,
                    messages = openAiMessages
                )

                val jsonBody = gson.toJson(requestBody)
                val mediaType = "application/json; charset=utf-8".toMediaType()

                val request = Request.Builder()
                    .url(apiUrl)
                    .addHeader("Authorization", "Bearer $apiKey")
                    .addHeader("Content-Type", "application/json")
                    .post(jsonBody.toRequestBody(mediaType))
                    .build()

                val response = client.newCall(request).execute()
                val responseBody = response.body?.string()

                if (!response.isSuccessful) {
                    val errorMsg = try {
                        val errorResponse = gson.fromJson(responseBody, OpenAiResponse::class.java)
                        errorResponse.error?.message ?: "HTTP ${response.code}"
                    } catch (e: Exception) {
                        "HTTP ${response.code}: ${response.message}"
                    }
                    return@withContext Result.failure(Exception(errorMsg))
                }

                val aiResponse = gson.fromJson(responseBody, OpenAiResponse::class.java)
                val reply = aiResponse.choices?.firstOrNull()?.message?.content
                    ?: return@withContext Result.failure(Exception("Порожня відповідь від AI"))

                return@withContext Result.success(reply)
            } catch (e: Exception) {
                return@withContext Result.failure(e)
            }
        }
    }
}
