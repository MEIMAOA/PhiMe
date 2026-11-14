#include "mod.h"

// Autoplay: hook each note control's Judge() and, before the game's own logic runs,
// mark the note as a perfect touch (ChartNote.isJudged = true and realTime = nowTime so
// the timing offset is 0). The game then scores it as its own Perfect judgement.
// autoRank stops that once the score reaches the chosen rank, so the rest miss.

using namespace il2cpp;

typedef bool (*JudgeFn)(Obj*, MethodInfo*);

static JudgeFn orig_click = nullptr, orig_drag = nullptr, orig_flick = nullptr, orig_hold = nullptr;
static bool g_ready = false;

// field offsets (game 4.0.1.2)
static const int kNoteClick = 0x40;   // ClickControl.noteInfor
static const int kNoteOther = 0x38;   // Drag/Flick/HoldControl.noteInfor
static const int kProgress = 0x28;    // every note control's progressControl
static const int kChartIsJudged = 0x28;  // ChartNote.isJudged
static const int kChartRealTime = 0x2C;  // ChartNote.realTime
static const int kNowTime = 0x90;        // ProgressControl.nowTime
static const int kScoreControl = 0x28;   // ProgressControl.scoreControl
static const int kScore = 0x40;          // ScoreControl._score

static float rankScore(int r) {
    switch (r) {
        case 1: return 960000.f;  // V
        case 2: return 920000.f;  // S
        case 3: return 880000.f;  // A
        case 4: return 820000.f;  // B
        case 5: return 700000.f;  // C
    }
    return 0.f;
}

static bool judgeHook(Obj* self, int noteOff, JudgeFn orig) {
    if (g_cfg.autoplay) {
        Obj* n = fld<Obj*>(self, noteOff);      // ChartNote*
        Obj* pc = fld<Obj*>(self, kProgress);  // ProgressControl*
        if (n && pc) {
            if (g_cfg.autoRank > 0) {
                Obj* sc = fld<Obj*>(pc, kScoreControl);
                if (sc && fld<float>(sc, kScore) >= rankScore(g_cfg.autoRank)) {
                    return orig(self, nullptr);  // reached the target: let the rest miss
                }
            }
            fld<bool>(n, kChartIsJudged) = true;
            fld<float>(n, kChartRealTime) = fld<float>(pc, kNowTime);  // perfect timing
            static int logged = 0;
            if (logged < 6) { plog("autoplay: judged note (self=%p)", (void*)self); logged++; }
        }
    }
    return orig(self, nullptr);
}

static bool hk_click(Obj* s, MethodInfo* m) { (void)m; return judgeHook(s, kNoteClick, orig_click); }
static bool hk_drag(Obj* s, MethodInfo* m) { (void)m; return judgeHook(s, kNoteOther, orig_drag); }
static bool hk_flick(Obj* s, MethodInfo* m) { (void)m; return judgeHook(s, kNoteOther, orig_flick); }
static bool hk_hold(Obj* s, MethodInfo* m) { (void)m; return judgeHook(s, kNoteOther, orig_hold); }

bool autoplayInstall() {
    Il2CppClass* Click = il2cpp::findClass("", "ClickControl");
    Il2CppClass* Drag = il2cpp::findClass("", "DragControl");
    Il2CppClass* Flick = il2cpp::findClass("", "FlickControl");
    Il2CppClass* Hold = il2cpp::findClass("", "HoldControl");
    MethodInfo* jClick = Click ? il2cpp::findMethod(Click, "Judge", 0) : nullptr;
    MethodInfo* jDrag = Drag ? il2cpp::findMethod(Drag, "Judge", 0) : nullptr;
    MethodInfo* jFlick = Flick ? il2cpp::findMethod(Flick, "Judge", 0) : nullptr;
    MethodInfo* jHold = Hold ? il2cpp::findMethod(Hold, "Judge", 0) : nullptr;

    if (jClick) orig_click = (JudgeFn)il2cpp::hookMethod(jClick, (void*)&hk_click);
    if (jDrag) orig_drag = (JudgeFn)il2cpp::hookMethod(jDrag, (void*)&hk_drag);
    if (jFlick) orig_flick = (JudgeFn)il2cpp::hookMethod(jFlick, (void*)&hk_flick);
    if (jHold) orig_hold = (JudgeFn)il2cpp::hookMethod(jHold, (void*)&hk_hold);

    g_ready = orig_click && orig_drag && orig_flick && orig_hold;
    plog("autoplay: %s (click=%d drag=%d flick=%d hold=%d)", g_ready ? "ready" : "partial", orig_click != nullptr,
         orig_drag != nullptr, orig_flick != nullptr, orig_hold != nullptr);
    return g_ready;
}
