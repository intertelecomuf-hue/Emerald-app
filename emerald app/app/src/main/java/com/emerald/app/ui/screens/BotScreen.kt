package com.emerald.app.ui.screens

import androidx.compose.foundation.layout.*
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.items
import androidx.compose.foundation.lazy.rememberLazyListState
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.text.KeyboardActions
import androidx.compose.foundation.text.KeyboardOptions
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.automirrored.filled.Send
import androidx.compose.material.icons.filled.SmartToy
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.platform.LocalConfiguration
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.input.ImeAction
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.emerald.app.ai.ChatMessage
import com.emerald.app.ai.LocalAi
import com.emerald.app.ui.theme.EmeraldPrimary
import kotlinx.coroutines.delay
import kotlinx.coroutines.launch

@Composable
fun BotScreen() {
    var messages by remember { mutableStateOf(listOf<ChatMessage>()) }
    var inputText by remember { mutableStateOf("") }
    var isLoading by remember { mutableStateOf(false) }

    val scope = rememberCoroutineScope()
    val listState = rememberLazyListState()

    // Автоскрол до нових повідомлень
    LaunchedEffect(messages.size) {
        if (messages.isNotEmpty()) {
            listState.animateScrollToItem(messages.size - 1)
        }
    }

    Column(
        modifier = Modifier
            .fillMaxSize()
            .padding(horizontal = 16.dp)
            .padding(top = 8.dp)
    ) {
        // Header
        Row(
            modifier = Modifier.fillMaxWidth(),
            verticalAlignment = Alignment.CenterVertically
        ) {
            Icon(
                imageVector = Icons.Default.SmartToy,
                contentDescription = null,
                modifier = Modifier.size(32.dp),
                tint = EmeraldPrimary
            )
            Spacer(modifier = Modifier.width(8.dp))
            Text(
                text = "BotShe",
                fontSize = 20.sp,
                fontWeight = FontWeight.Bold,
                color = EmeraldPrimary,
                modifier = Modifier.weight(1f)
            )
            // Індикатор "офлайн/безкоштовно"
            Surface(
                shape = RoundedCornerShape(12.dp),
                color = EmeraldPrimary.copy(alpha = 0.1f)
            ) {
                Text(
                    text = "Безкоштовно",
                    modifier = Modifier.padding(horizontal = 10.dp, vertical = 4.dp),
                    fontSize = 11.sp,
                    color = EmeraldPrimary,
                    fontWeight = FontWeight.Medium
                )
            }
        }

        Spacer(modifier = Modifier.height(4.dp))

        // Підпис
        Text(
            text = "Працює без інтернету та API-ключів",
            fontSize = 12.sp,
            color = MaterialTheme.colorScheme.onSurface.copy(alpha = 0.4f)
        )

        Spacer(modifier = Modifier.height(8.dp))

        // Чат
        if (messages.isEmpty()) {
            // Пустий стан
            Box(
                modifier = Modifier
                    .weight(1f)
                    .fillMaxWidth(),
                contentAlignment = Alignment.Center
            ) {
                Column(
                    horizontalAlignment = Alignment.CenterHorizontally
                ) {
                    Icon(
                        imageVector = Icons.Default.SmartToy,
                        contentDescription = null,
                        modifier = Modifier.size(80.dp),
                        tint = EmeraldPrimary.copy(alpha = 0.5f)
                    )
                    Spacer(modifier = Modifier.height(16.dp))
                    Text(
                        text = "Спілкуйся з BotShe!",
                        style = MaterialTheme.typography.titleMedium,
                        color = MaterialTheme.colorScheme.onSurface.copy(alpha = 0.5f)
                    )
                    Spacer(modifier = Modifier.height(8.dp))
                    Text(
                        text = "Напиши щось, щоб почати розмову",
                        style = MaterialTheme.typography.bodyMedium,
                        color = MaterialTheme.colorScheme.onSurface.copy(alpha = 0.3f)
                    )
                    Spacer(modifier = Modifier.height(24.dp))

                    // Підказки
                    Text(
                        text = "Спробуй запитати:",
                        style = MaterialTheme.typography.bodySmall,
                        color = MaterialTheme.colorScheme.onSurface.copy(alpha = 0.4f)
                    )
                    Spacer(modifier = Modifier.height(8.dp))

                    listOf(
                        "Привіт!",
                        "Розкажи жарт",
                        "Як справи?",
                        "Цікавий факт"
                    ).forEach { hint ->
                        SuggestionChip(
                            onClick = {
                                messages = messages + ChatMessage("user", hint)
                                isLoading = true
                                scope.launch {
                                    val reply = LocalAi.getResponse(hint, messages)
                                    delay(500) // Невелика затримка для природності
                                    messages = messages + ChatMessage("assistant", reply)
                                    isLoading = false
                                }
                            },
                            label = { Text(hint, fontSize = 13.sp) },
                            modifier = Modifier.padding(vertical = 2.dp),
                            colors = SuggestionChipDefaults.suggestionChipColors(
                                containerColor = EmeraldPrimary.copy(alpha = 0.08f)
                            )
                        )
                        Spacer(modifier = Modifier.height(4.dp))
                    }
                }
            }
        } else {
            // Повідомлення
            LazyColumn(
                modifier = Modifier
                    .weight(1f)
                    .fillMaxWidth(),
                state = listState,
                verticalArrangement = Arrangement.spacedBy(8.dp),
                contentPadding = PaddingValues(vertical = 8.dp)
            ) {
                items(messages) { message ->
                    ChatBubble(message = message)
                }

                if (isLoading) {
                    item {
                        TypingIndicator()
                    }
                }
            }
        }

        Spacer(modifier = Modifier.height(8.dp))

        // Поле вводу
        Card(
            modifier = Modifier.fillMaxWidth(),
            colors = CardDefaults.cardColors(
                containerColor = MaterialTheme.colorScheme.surfaceVariant.copy(alpha = 0.5f)
            ),
            shape = RoundedCornerShape(24.dp)
        ) {
            Row(
                modifier = Modifier
                    .fillMaxWidth()
                    .padding(horizontal = 16.dp, vertical = 4.dp),
                verticalAlignment = Alignment.CenterVertically
            ) {
                OutlinedTextField(
                    value = inputText,
                    onValueChange = { inputText = it },
                    modifier = Modifier.weight(1f),
                    placeholder = { Text("Напиши повідомлення...") },
                    singleLine = true,
                    colors = OutlinedTextFieldDefaults.colors(
                        focusedBorderColor = Color.Transparent,
                        unfocusedBorderColor = Color.Transparent,
                        focusedContainerColor = Color.Transparent,
                        unfocusedContainerColor = Color.Transparent
                    ),
                    keyboardOptions = KeyboardOptions(imeAction = ImeAction.Send),
                    keyboardActions = KeyboardActions(
                        onSend = { sendMessage(inputText, messages, isLoading, scope, { messages = it }, { isLoading = it }, { inputText = it }) }
                    )
                )

                Spacer(modifier = Modifier.width(8.dp))

                FilledIconButton(
                    onClick = { sendMessage(inputText, messages, isLoading, scope, { messages = it }, { isLoading = it }, { inputText = it }) },
                    enabled = inputText.isNotBlank() && !isLoading,
                    colors = IconButtonDefaults.filledIconButtonColors(
                        containerColor = EmeraldPrimary
                    )
                ) {
                    Icon(
                        imageVector = Icons.AutoMirrored.Filled.Send,
                        contentDescription = "Надіслати",
                        tint = Color.White
                    )
                }
            }
        }

        Spacer(modifier = Modifier.height(8.dp))
    }
}

private fun sendMessage(
    inputText: String,
    messages: List<ChatMessage>,
    isLoading: Boolean,
    scope: kotlinx.coroutines.CoroutineScope,
    setMessages: (List<ChatMessage>) -> Unit,
    setIsLoading: (Boolean) -> Unit,
    setInputText: (String) -> Unit
) {
    if (inputText.isNotBlank() && !isLoading) {
        val userMessage = inputText.trim()
        setInputText("")
        setMessages(messages + ChatMessage("user", userMessage))
        setIsLoading(true)

        scope.launch {
            val reply = LocalAi.getResponse(userMessage, messages)
            delay(500) // Невелика затримка, щоб було відчуття "живого" спілкування
            setMessages(messages + ChatMessage("user", userMessage) + ChatMessage("assistant", reply))
            setIsLoading(false)
        }
    }
}

@Composable
private fun ChatBubble(message: ChatMessage) {
    val isUser = message.role == "user"

    Row(
        modifier = Modifier.fillMaxWidth(),
        horizontalArrangement = if (isUser) Arrangement.End else Arrangement.Start
    ) {
        Card(
            modifier = Modifier
                .widthIn(max = LocalConfiguration.current.screenWidthDp.dp * 0.75f),
            colors = CardDefaults.cardColors(
                containerColor = if (isUser) EmeraldPrimary
                else MaterialTheme.colorScheme.surfaceVariant
            ),
            shape = RoundedCornerShape(
                topStart = 16.dp,
                topEnd = 16.dp,
                bottomStart = if (isUser) 16.dp else 4.dp,
                bottomEnd = if (isUser) 4.dp else 16.dp
            )
        ) {
            Text(
                text = message.content,
                modifier = Modifier.padding(12.dp),
                style = MaterialTheme.typography.bodyMedium,
                color = if (isUser) Color.White
                else MaterialTheme.colorScheme.onSurface
            )
        }
    }
}

@Composable
private fun TypingIndicator() {
    Row(
        modifier = Modifier.fillMaxWidth(),
        horizontalArrangement = Arrangement.Start
    ) {
        Card(
            colors = CardDefaults.cardColors(
                containerColor = MaterialTheme.colorScheme.surfaceVariant
            ),
            shape = RoundedCornerShape(
                topStart = 16.dp,
                topEnd = 16.dp,
                bottomStart = 4.dp,
                bottomEnd = 16.dp
            )
        ) {
            Text(
                text = "BotShe друкує...",
                modifier = Modifier.padding(12.dp),
                style = MaterialTheme.typography.bodyMedium,
                color = MaterialTheme.colorScheme.onSurface.copy(alpha = 0.5f)
            )
        }
    }
}
