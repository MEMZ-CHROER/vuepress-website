-- lxy-admin D1 数据库表结构
-- 初始化：
--   wrangler d1 execute lxy-admin --remote --file=./schema.sql
-- 建管理员：
--   wrangler d1 execute lxy-admin --remote --command "INSERT INTO users (username, password_hash, role, permissions) VALUES ('admin', '<SHA256哈希>', 'admin', 'all')"

CREATE TABLE IF NOT EXISTS users (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  username TEXT NOT NULL UNIQUE,
  password_hash TEXT NOT NULL,
  role TEXT NOT NULL DEFAULT 'editor',
  permissions TEXT NOT NULL DEFAULT 'media,navbar,pages,passwords',
  write_paths TEXT NOT NULL DEFAULT '',
  created_at TEXT NOT NULL DEFAULT (datetime('now'))
);

CREATE TABLE IF NOT EXISTS sessions (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  user_id INTEGER NOT NULL,
  token TEXT NOT NULL UNIQUE,
  created_at TEXT NOT NULL DEFAULT (datetime('now')),
  FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE
);

CREATE INDEX IF NOT EXISTS idx_sessions_token ON sessions(token);
CREATE INDEX IF NOT EXISTS idx_sessions_user ON sessions(user_id);
