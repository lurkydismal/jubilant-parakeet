For a 2D fighting game, especially one that supports 1–4 players, the camera system needs to do the following:

---

### ✅ **Camera Goals**

* **Always frame all active players.**
* **Zoom in when players are close.**
* **Zoom out when players are far apart (up to a max).**
* **Clamp camera position to stage bounds.**
* **Be stable and not jittery.**

---

### 🧠 **Camera Data Structure**

```c
typedef struct {
    float x, y;         // Camera center
    float zoom;         // 1.0 = default
    float min_zoom;
    float max_zoom;
    float view_width;
    float view_height;
} Camera2D;
```

---

### 🎯 **Step-by-Step Camera Logic**

#### 1. **Find Player Bounds**

Calculate a bounding box around all active players:

```c
float min_x = FLT_MAX, min_y = FLT_MAX;
float max_x = -FLT_MAX, max_y = -FLT_MAX;

for (int _i = 0; _i < num_players; ++_i) {
    if (!players[_i].active) continue;
    float px = players[_i].pos.x;
    float py = players[_i].pos.y;
    min_x = fminf(min_x, px);
    max_x = fmaxf(max_x, px);
    min_y = fminf(min_y, py);
    max_y = fmaxf(max_y, py);
}
```

#### 2. **Center Camera**

```c
camera->x = (min_x + max_x) * 0.5f;
camera->y = (min_y + max_y) * 0.5f;
```

#### 3. **Adjust Zoom**

```c
float margin = 100.0f; // pixels
float span_x = (max_x - min_x) + margin;
float span_y = (max_y - min_y) + margin;

float zoom_x = camera->view_width / span_x;
float zoom_y = camera->view_height / span_y;

float target_zoom = fminf(zoom_x, zoom_y);
target_zoom = fminf(camera->max_zoom, fmaxf(camera->min_zoom, target_zoom));

// Smooth zooming
camera->zoom += (target_zoom - camera->zoom) * 0.1f;
```

#### 4. **Clamp to Stage**

If your stage is, say, `stage_width x stage_height`:

```c
float half_width = (camera->view_width / camera->zoom) * 0.5f;
float half_height = (camera->view_height / camera->zoom) * 0.5f;

float left = half_width;
float right = stage_width - half_width;
float top = half_height;
float bottom = stage_height - half_height;

camera->x = fminf(fmaxf(camera->x, left), right);
camera->y = fminf(fmaxf(camera->y, top), bottom);
```

---

### 🛠️ **Rendering**

When drawing, offset everything by the camera:

```c
float cam_left = camera->x - camera->view_width * 0.5f / camera->zoom;
float cam_top = camera->y - camera->view_height * 0.5f / camera->zoom;

// Set your orthographic projection using cam_left, cam_top and zoom
```

---

### 💡 Tips

* Set a **maximum zoom-out** to prevent players from being too tiny.
* If only 1 player is alive, **center on them** with a default zoom.
* For smoother visuals, lerp position like zoom:
  `camera->x += (target_x - camera->x) * 0.1f;`

---

If you want help integrating this into your code or want multiplayer splitscreen fallback, let me know.

