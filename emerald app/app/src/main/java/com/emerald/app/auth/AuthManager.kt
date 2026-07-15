package com.emerald.app.auth

sealed class AuthResult {
    data class Success(val email: String) : AuthResult()
    data class Error(val message: String) : AuthResult()
}

/**
 * Локальний менеджер авторизації (синглтон).
 * Зберігає дані в пам'яті.
 */
object AuthManager {
    private val users = mutableMapOf<String, String>() // email -> password
    private var _currentUserEmail: String? = null

    val currentUserEmail: String?
        get() = _currentUserEmail

    val isUserLoggedIn: Boolean
        get() = _currentUserEmail != null

    suspend fun login(email: String, password: String): AuthResult {
        kotlinx.coroutines.delay(1000)

        val normalizedEmail = email.lowercase().trim()
        val storedPassword = users[normalizedEmail]
        return if (storedPassword != null && storedPassword == password) {
            _currentUserEmail = normalizedEmail
            AuthResult.Success(normalizedEmail)
        } else if (storedPassword == null) {
            AuthResult.Error("Користувача з таким email не знайдено")
        } else {
            AuthResult.Error("Невірний пароль")
        }
    }

    suspend fun register(email: String, password: String): AuthResult {
        kotlinx.coroutines.delay(500)

        val normalizedEmail = email.lowercase().trim()
        return if (users.containsKey(normalizedEmail)) {
            AuthResult.Error("Користувач з таким email вже існує")
        } else {
            users[normalizedEmail] = password
            _currentUserEmail = normalizedEmail
            AuthResult.Success(normalizedEmail)
        }
    }

    fun logout() {
        _currentUserEmail = null
    }
}
