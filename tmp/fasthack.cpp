//
// processed\../src/apDebug.cpp
//

#include <game.h>

class APDebugDrawer : public m3d::proc_c {
	public:
		APDebugDrawer();

		bool amISetUp;
		mHeapAllocator_c allocator;

		void setMeUp();

		void drawMe();

		void drawOpa();
		void drawXlu();
};


static APDebugDrawer defaultInstance;
static bool enableDebugMode = false;

int APDebugDraw() {
	if (enableDebugMode)
		defaultInstance.drawMe();
	return 1;
}


APDebugDrawer::APDebugDrawer() {
	amISetUp = false;
}

void APDebugDrawer::setMeUp() {
	allocator.setup(GameHeaps[0], 0x20);
	setup(&allocator);
}

void APDebugDrawer::drawMe() {
	if (!amISetUp) {
		setMeUp();
		amISetUp = true;
	}

	scheduleForDrawing();
}

void APDebugDrawer::drawOpa() {
	drawXlu();
}
void APDebugDrawer::drawXlu() {
	GXClearVtxDesc();

	GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
	GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT);

	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);

	GXSetNumIndStages(0);
	for (int i = 0; i < 0x10; i++)
		GXSetTevDirect(i);

	GXSetNumChans(1);
	GXSetChanCtrl(GX_COLOR0A0, GX_DISABLE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHTNULL, GX_DF_NONE, GX_AF_NONE);
	GXSetChanAmbColor(GX_COLOR0A0, (GXColor){255,255,255,255});
	GXSetChanMatColor(GX_COLOR0A0, (GXColor){255,255,255,255});
	GXSetNumTexGens(0);

	GXSetNumTevStages(1);
	GXSetNumIndStages(0);

	GXSetTevSwapModeTable(GX_TEV_SWAP0, GX_CH_RED, GX_CH_GREEN, GX_CH_BLUE, GX_CH_ALPHA);

	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0A0);
	GXSetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
//	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_C1, GX_CC_C0, GX_CC_RASC, GX_CC_ZERO);
//	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
//	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_RASA, GX_CA_ZERO);
//	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);

	GXSetZCompLoc(GX_FALSE);
	GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_SET);
	GXSetZMode(GX_TRUE, GX_ALWAYS, GX_FALSE);
	GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_OR, GX_ALWAYS, 0);

	GXSetFog(GX_FOG_NONE, 0, 0, 0, 0, (GXColor){0,0,0,0});
	GXSetFogRangeAdj(GX_FALSE, 0, 0);

	GXSetCullMode(GX_CULL_NONE);

	GXSetDither(GX_TRUE);
	GXSetLineWidth(18, GX_TO_ZERO);

	GXSetTevColor(GX_TEVREG0, (GXColor){255,255,255,255});
	GXSetTevColor(GX_TEVREG1, (GXColor){0,0,0,255});

	nw4r::g3d::Camera cam(GetCameraByID(GetCurrentCameraID()));
	Mtx matrix;
	cam.GetCameraMtx(&matrix);
	GXLoadPosMtxImm(matrix, 0);
	GXSetCurrentMtx(0);

	ActivePhysics *ap = ActivePhysics::globalListHead;

	while (ap) {
//		if (ap->owner->name == PLAYER)
//			OSReport("Player has : DistToC=%f,%f DistToEdge=%f,%f Pos=%f,%f Scale=%f,%f\n",
//					ap->info.xDistToCenter, ap->info.yDistToCenter,
//					ap->info.xDistToEdge, ap->info.yDistToEdge,
//					ap->owner->pos.x, ap->owner->pos.y,
//					ap->owner->scale.x, ap->owner->scale.y);

		u32 uptr = (u32)ap;
		u8 r = (uptr>>16)&0xFF;
		u8 g = (uptr>>8)&0xFF;
		u8 b = uptr&0xFF;
		u8 a = 0xFF;

		GXBegin(GX_LINES, GX_VTXFMT0, 10);

		float centreX = ap->owner->pos.x + ap->info.xDistToCenter;
		float centreY = ap->owner->pos.y + ap->info.yDistToCenter;
		float edgeDistX = ap->info.xDistToEdge;
		float edgeDistY = ap->info.yDistToEdge;

		float tlX = centreX - edgeDistX, tlY = centreY + edgeDistY;
		float trX = centreX + edgeDistX, trY = centreY + edgeDistY;

		float blX = centreX - edgeDistX, blY = centreY - edgeDistY;
		float brX = centreX + edgeDistX, brY = centreY - edgeDistY;

		switch (ap->collisionCheckType) {
			case 2: // vert trapezoid
				tlY = centreY + ap->trpValue0;
				trY = centreY + ap->trpValue1;
				blY = centreY + ap->trpValue2;
				brY = centreY + ap->trpValue3;
				break;
			case 3: // horz trapezoid
				tlX = centreX + ap->trpValue0;
				trX = centreX + ap->trpValue1;
				blX = centreX + ap->trpValue2;
				brX = centreX + ap->trpValue3;
				break;
		}

		// Top
		GXPosition3f32(tlX, tlY, 9000.0f);
		GXColor4u8(r,g,b,a);
		GXPosition3f32(trX, trY, 9000.0f);
		GXColor4u8(r,g,b,a);

		// Left
		GXPosition3f32(tlX, tlY, 9000.0f);
		GXColor4u8(r,g,b,a);
		GXPosition3f32(blX, blY, 9000.0f);
		GXColor4u8(r,g,b,a);

		// Right
		GXPosition3f32(trX, trY, 9000.0f);
		GXColor4u8(r,g,b,a);
		GXPosition3f32(brX, brY, 9000.0f);
		GXColor4u8(r,g,b,a);

		// Bottom
		GXPosition3f32(blX, blY, 9000.0f);
		GXColor4u8(r,g,b,a);
		GXPosition3f32(brX, brY, 9000.0f);
		GXColor4u8(r,g,b,a);

		// Diagonal
		GXPosition3f32(trX, trY, 9000.0f);
		GXColor4u8(r,g,b,a);
		GXPosition3f32(blX, blY, 9000.0f);
		GXColor4u8(r,g,b,a);

		GXEnd();

		ap = ap->listPrev;
	}

	Physics *p = Physics::globalListHead;

	while (p) {
		u32 uptr = (u32)p;
		u8 r = (uptr>>16)&0xFF;
		u8 g = (uptr>>8)&0xFF;
		u8 b = uptr&0xFF;
		u8 a = 0xFF;

		GXBegin(GX_LINES, GX_VTXFMT0, 10);

		float tlX = p->unkArray[0].x;
		float tlY = p->unkArray[0].y;
		float trX = p->unkArray[3].x;
		float trY = p->unkArray[3].y;
		float blX = p->unkArray[1].x;
		float blY = p->unkArray[1].y;
		float brX = p->unkArray[2].x;
		float brY = p->unkArray[2].y;

		// Top
		GXPosition3f32(tlX, tlY, 9000.0f);
		GXColor4u8(r,g,b,a);
		GXPosition3f32(trX, trY, 9000.0f);
		GXColor4u8(r,g,b,a);

		// Left
		GXPosition3f32(tlX, tlY, 9000.0f);
		GXColor4u8(r,g,b,a);
		GXPosition3f32(blX, blY, 9000.0f);
		GXColor4u8(r,g,b,a);

		// Right
		GXPosition3f32(trX, trY, 9000.0f);
		GXColor4u8(r,g,b,a);
		GXPosition3f32(brX, brY, 9000.0f);
		GXColor4u8(r,g,b,a);

		// Bottom
		GXPosition3f32(blX, blY, 9000.0f);
		GXColor4u8(r,g,b,a);
		GXPosition3f32(brX, brY, 9000.0f);
		GXColor4u8(r,g,b,a);

		// Diagonal
		GXPosition3f32(trX, trY, 9000.0f);
		GXColor4u8(r,g,b,a);
		GXPosition3f32(blX, blY, 9000.0f);
		GXColor4u8(r,g,b,a);

		GXEnd();

		p = p->next;
	}


	// Basic Colliders
	BasicCollider *bc = BasicCollider::globalListHead;
	while (bc) {
		u32 uptr = (u32)bc;
		u8 r = (uptr>>16)&0xFF;
		u8 g = (uptr>>8)&0xFF;
		u8 b = uptr&0xFF;
		u8 a = 0xFF;

		switch (bc->type) {
			case 0: case 2:
				GXBegin(GX_LINES, GX_VTXFMT0, 2);
				GXPosition3f32(bc->leftX, bc->leftY, 9000.0f);
				GXColor4u8(r,g,b,a);
				GXPosition3f32(bc->rightX, bc->rightY, 9000.0f);
				GXColor4u8(r,g,b,a);
				GXEnd();
				break;
		}

		bc = bc->next;
	}


	// Now, the hardest one.. CollisionMgr_c!
	fBase_c *fb = 0;
	while ((fb = fBase_c::searchByBaseType(2, fb))) {
		u8 *testMe = ((u8*)fb) + 0x1EC;
		if (*((u32*)testMe) != 0x8030F6D0)
			continue;

		u32 uptr = (u32)fb;
		u8 r = u8((uptr>>16)&0xFF)+0x20;
		u8 g = u8((uptr>>8)&0xFF)-0x30;
		u8 b = u8(uptr&0xFF)+0x80;
		u8 a = 0xFF;

		dStageActor_c *ac = (dStageActor_c*)fb;

		sensorBase_s *sensors[4] = {
			ac->collMgr.pBelowInfo, ac->collMgr.pAboveInfo,
			ac->collMgr.pAdjacentInfo, ac->collMgr.pAdjacentInfo};

		for (int i = 0; i < 4; i++) {
			sensorBase_s *s = sensors[i];
			if (!s)
				continue;

			float mult = (i == 3) ? -1.0f : 1.0f;

			switch (s->flags & SENSOR_TYPE_MASK) {
				case SENSOR_POINT:
					GXBegin(GX_POINTS, GX_VTXFMT0, 1);
					GXPosition3f32(
							ac->pos.x + (mult * (s->asPoint()->x / 4096.0f)),
							ac->pos.y + (s->asPoint()->y / 4096.0f),
							8005.0f);
					GXColor4u8(r,g,b,a);
					GXEnd();
					break;
				case SENSOR_LINE:
					GXBegin(GX_LINES, GX_VTXFMT0, 2);
					if (i < 2) {
						GXPosition3f32(
								ac->pos.x + (s->asLine()->lineA / 4096.0f),
								ac->pos.y + (s->asLine()->distanceFromCenter / 4096.0f),
								8005.0f);
						GXColor4u8(r,g,b,a);
						GXPosition3f32(
								ac->pos.x + (s->asLine()->lineB / 4096.0f),
								ac->pos.y + (s->asLine()->distanceFromCenter / 4096.0f),
								8005.0f);
						GXColor4u8(r,g,b,a);
					} else {
						GXPosition3f32(
								ac->pos.x + (mult * (s->asLine()->distanceFromCenter / 4096.0f)),
								ac->pos.y + (s->asLine()->lineA / 4096.0f),
								8005.0f);
						GXColor4u8(r,g,b,a);
						GXPosition3f32(
								ac->pos.x + (mult * (s->asLine()->distanceFromCenter / 4096.0f)),
								ac->pos.y + (s->asLine()->lineB / 4096.0f),
								8005.0f);
						GXColor4u8(r,g,b,a);
					}
					GXEnd();
					break;
			}
		}
	}
}

//
// processed\../src/creditsMgr.cpp
//

#include <game.h>
#include <sfx.h>
#include <dCourse.h>
#include <stage.h>
#include <playeranim.h>
#include <newer.h>
void *EGG__Heap__alloc(unsigned long size, int unk, void *heap);
void EGG__Heap__free(void *ptr, void *heap);

extern char CameraLockEnabled;
extern VEC2 CameraLockPosition;

extern char isLockPlayerRotation;
extern s16 lockedPlayerRotation;

extern bool NoMichaelBuble;

mTexture_c efbTexture;
bool getNextEFB = false;
int thing = 0;

const char *CreditsFileList[] = {"CreditsBG", 0};

extern "C" void GXPixModeSync();
extern "C" void *MakeMarioEnterDemoMode();
extern "C" void *MakeMarioExitDemoMode();

struct FireworkInfo {
	const char *name;
	float xOffset, yOffset;
	int delay;
};

extern void *SoundRelatedClass;

extern u16 DanceValues_AnimSpeed; // 80427C2E
extern u8 DanceValues_DummyBlockAndGrass; // 8042A049
extern u8 DanceValues_Bahps; // 8042A04A
extern u8 DanceValues_CreditsControl; // 8042A04B

class dFlipbookRenderer_c : public m3d::proc_c {
	public:
		dFlipbookRenderer_c();
		~dFlipbookRenderer_c();

		mAllocator_c allocator;
		void drawOpa();
		void drawXlu();
		void execute();

		bool isEnabled;
		int flipFrame;

		int scheduledBG;

		char *tplBuffer[2];
		u32 tplBufferSize[2];
		GXTexObj bgTexObj[2];

		void loadNewBG(int bgID, bool isBackface);
};

struct danceInfo_s {
	u32 when;
	u8 animSpeed, dummyBlockFlag, bahpFlag, creditsFlag;
};

class dCreditsMgr_c : public dActorState_c {
	public:
		int onCreate();
		int onDelete();
		int onExecute();
		int onDraw();

		int currentPathID;

		bool isOutOfView() { return false; }
		Vec2 _vf70();

		dDvdLoader_c scriptLoader;
		const u8 *scriptPos;

		dFlipbookRenderer_c renderer;

		bool loadLayout();
		bool loadTitleLayout();
		bool layoutLoaded;
		bool titleLayoutLoaded;
		m2d::EmbedLayout_c layout;
		m2d::EmbedLayout_c titleLayout;

		bool titleLayoutVisible;

		int countdown;

		bool fireworks;
		int fireworksCountdown;
		int fwID;

		int fauxScrollFrame;
		float fauxScrollMinX, fauxScrollMaxX, fauxScrollY;

		VEC2 endingLockPositions[4];

		danceInfo_s *danceCommand;
		int danceTimer;

		nw4r::lyt::TextBox
			*Title, *TitleS,
			*Name, *NameS,
			*LeftName, *LeftNameS,
			*RightName, *RightNameS;
		nw4r::lyt::Pane
			*TitleContainer, *NamesContainer,
			*OneNameC, *TwoNamesC, *N_proportionC_00;

		void doAutoscroll(int pathID);
		void positionPlayers();

		void animComplete();

		void enableZoom();
		void disableZoom();
		void playerWinAnim();
		void playerLookUp();
		void theEnd();
		void exitStage();

		bool endingMode;

//		USING_STATES(dCreditsMgr_c);
//		DECLARE_STATE(Wait);
//		DECLARE_STATE(PlayLayoutAnim);
//		DECLARE_STATE(Flipping);

		static dCreditsMgr_c *build();
		static dCreditsMgr_c *instance;
};
// CREATE_STATE(dCreditsMgr_c, Wait);
// CREATE_STATE(dCreditsMgr_c, PlayLayoutAnim);
// CREATE_STATE(dCreditsMgr_c, Flipping);

dCreditsMgr_c *dCreditsMgr_c::instance = 0;

dCreditsMgr_c *dCreditsMgr_c::build() {
	void *buf = AllocFromGameHeap1(sizeof(dCreditsMgr_c));
	return new(buf) dCreditsMgr_c;
}


int dCreditsMgr_c::onCreate() {
	NoMichaelBuble = true;

	instance = this;

	if (!loadLayout())
		return false;
	if (!loadTitleLayout())
		return false;

	if (!scriptLoader.load("/NewerRes/NewerStaffRoll.bin"))
		return false;

	scriptPos = (const u8*)scriptLoader.buffer;

	getNextEFB = true; // make sure we have a texture

	renderer.allocator.setup(GameHeaps[0], 0x20);
	bool result = renderer.setup(&renderer.allocator);

	renderer.loadNewBG(0, false);

	//acState.setState(&StateID_Wait);

	return true;
}

int dCreditsMgr_c::onDelete() {
	instance = 0;

	isLockPlayerRotation = false;

	scriptLoader.unload();
	return layout.free() && titleLayout.free();
}
extern "C" bool SpawnEffect(const char*, int, Vec*, S16Vec*, Vec*);
int dCreditsMgr_c::onExecute() {
	danceTimer++;
	if (danceCommand == 0)
		danceCommand = (danceInfo_s*)getResource("CreditsBG", "/Dance.bin");

	char *autoscrInfo = ((char*)dBgGm_c::instance) + 0x900AC;

	fauxScrollFrame++;
	if (fauxScrollFrame > 60)
		fauxScrollFrame = 60;
	float fsMult = (1.0f / 60.0f) * float(fauxScrollFrame);
	float interp = float(fauxScrollMinX) + ((fauxScrollMaxX - fauxScrollMinX) * fsMult);
	CameraLockPosition.x = interp;
	CameraLockPosition.y = fauxScrollY;

	if (endingMode) {
		for (int i = 0; i < 4; i++) {
			dAcPy_c *player;
			if ((player = dAcPy_c::findByID(i))) {
				if (!player->testFlag(0x24)) {
					player->setAnimePlayWithAnimID(0);
				}
				player->setFlag(0x24);
				player->rot.y = 0;
				player->speed.x = player->speed.y = player->speed.z = 0.0f;
				player->pos.x = endingLockPositions[i].x;
				player->pos.y = endingLockPositions[i].y;
			}
		}
	}

	if (fireworks) {
		fireworksCountdown--;
		if (fireworksCountdown <= 0) {
			static const FireworkInfo fwInfo[] = {
				{"Wm_ob_fireworks_g", 20.000000f, 49.000000f, 8}, // ends @ 8
				{"Wm_ob_fireworks_1up", 154.000000f, 80.000000f, 14}, // ends @ 22
				{"Wm_ob_fireworks_b", 168.000000f, 27.000000f, 33}, // ends @ 55
				{"Wm_ob_fireworks_1up", 416.000000f, 22.000000f, 33}, // ends @ 88
				{"Wm_ob_fireworks_y", 179.000000f, 11.000000f, 8}, // ends @ 96
				{"Wm_ob_fireworks_star", 9.000000f, 35.000000f, 25}, // ends @ 121
				{"Wm_ob_fireworks_y", 398.000000f, 29.000000f, 11}, // ends @ 132
				{"Wm_ob_fireworks_g", 127.000000f, 64.000000f, 21}, // ends @ 153
				{"Wm_ob_fireworks_star", 439.000000f, 66.000000f, 3}, // ends @ 156
				{"Wm_ob_fireworks_k", 320.000000f, 18.000000f, 31}, // ends @ 187
				{"Wm_ob_fireworks_p", 158.000000f, 47.000000f, 42}, // ends @ 229
				{"Wm_ob_fireworks_star", 127.000000f, 1.000000f, 29}, // ends @ 258
				{"Wm_ob_fireworks_k", 164.000000f, 50.000000f, 18}, // ends @ 276
				{"Wm_ob_fireworks_g", 365.000000f, 25.000000f, 39}, // ends @ 315
				{"Wm_ob_fireworks_k", 2.000000f, 78.000000f, 44}, // ends @ 359
				{"Wm_ob_fireworks_g", 309.000000f, 25.000000f, 42}, // ends @ 401
				{"Wm_ob_fireworks_star", 222.000000f, 78.000000f, 44}, // ends @ 445
				//{"Wm_ob_fireworks_y", 269.000000f, 23.000000f, 38}, // ends @ 483
				{0, 0.0f, 0.0f, 0},
			};

			fireworksCountdown = fwInfo[fwID].delay;
			float xOffs = fwInfo[fwID].xOffset;
			float yOffs = fwInfo[fwID].yOffset;
			VEC3 efPos = {10208.0f + xOffs, -304.0f - yOffs, pos.z + 200.0f};

			SpawnEffect(fwInfo[fwID].name, 0, &efPos, 0, 0);

			nw4r::snd::SoundHandle handle;
			PlaySoundWithFunctionB4(SoundRelatedClass, &handle, SE_OBJ_GOAL_FIREWORKS, 1);

			fwID++;
			if (!fwInfo[fwID].name)
				fireworks = false;
		}
	}

	if (countdown > 0) {
		countdown--;
	} else if (!renderer.isEnabled) {
		if (!endingMode)
			isLockPlayerRotation = false;

		// Execute commands if we can
		bool exitInterpreter = false;
		while (scriptPos && !exitInterpreter) {
			int u16_top, u16_bottom;
			int whatAnim;
			u8 *staffCreditScore;
			nw4r::lyt::Pane *paneThing;

			const u8 *read = scriptPos;
			int cmdSize = *(read++);
			int cmdType = *(read++);

			scriptPos += cmdSize;

			switch (cmdType) {
				case 0: // Stop running script
					scriptPos = 0;
					exitInterpreter = true;
					break;

				case 1: // Delay
					u16_top = *(read++);
					u16_bottom = *(read++);
					countdown = (u16_top << 8) | u16_bottom;
					exitInterpreter = true;
					break;

				case 2: // Switch scene
					doAutoscroll(*(read++));
					break;

				case 3: // Switch scene and wait
					doAutoscroll(*(read++));
					exitInterpreter = true;
					break;

				case 4: // Show scores
					staffCreditScore = GetStaffCreditScore();
					staffCreditScore[0x279] = 1;
					// Hide the high score bit
					paneThing = *((nw4r::lyt::Pane**)(staffCreditScore + 0x274));
					paneThing->SetVisible(false);
					OSReport("Staff Credit Score object is at %p, going to show it\n", staffCreditScore);
					break;

				case 5: // Show text
					layout.enableNonLoopAnim(0);
					break;
				case 6: // Hide text
					layout.enableNonLoopAnim(1);
					break;

				case 7: { // Set names. FUN!
					int titleLength = *(read++);
					int nameCount = *(read++);

					WriteAsciiToTextBox(Title, (const char*)read);
					WriteAsciiToTextBox(TitleS, (const char*)read);

					read += titleLength;

					WriteAsciiToTextBox(Name, (const char*)read);
					WriteAsciiToTextBox(NameS, (const char*)read);

					float calcHeight = 29.0f * nameCount;
					TitleContainer->trans.y = (calcHeight * 0.5f) + 3.0f;

					OneNameC->SetVisible(true);
					TwoNamesC->SetVisible(false);

					} break;

				case 8:
					titleLayoutVisible = true;
					break;
				case 9:
					titleLayoutVisible = false;
					break;
				case 10:
					whatAnim = *(read++);
					titleLayout.enableNonLoopAnim(whatAnim);
					break;

				case 11:
					endingMode = true;
					break;
				case 12:
					enableZoom();
					break;
				case 13:
					playerWinAnim();
					break;
				case 14:
					disableZoom();
					break;
				case 15:
					playerLookUp();
					break;
				case 16:
					theEnd();
					break;
				case 17:
					exitStage();
					exitInterpreter = true;
					break;
				case 18:
					GetTheEnd()->willHide = true;
					break;
				case 19:
					fireworks = true;
					fireworksCountdown = 25;
					break;
				case 20:
					fireworks = false;
					break;
			}
		}
	}

	layout.execAnimations();
	layout.update();

	titleLayout.execAnimations();
	titleLayout.update();

	//acState.execute();
	renderer.execute();

	return true;
}

int dCreditsMgr_c::onDraw() {
	renderer.scheduleForDrawing();
	layout.scheduleForDrawing();
	if (titleLayoutVisible)
		titleLayout.scheduleForDrawing();
	return true;
}


bool dCreditsMgr_c::loadLayout() {
	if (!layoutLoaded) {
		if (!layout.loadArc("StaffRoll.arc", false))
			return false;

		layout.build("StaffRoll.brlyt");

		static const char *brlanNames[] = {
			"StaffRoll_show.brlan",
			"StaffRoll_hide.brlan",
		};
		static const char *groupNames[] = {
			"TheOnlyGroup", "TheOnlyGroup"
		};

		layout.loadAnimations(brlanNames, 2);
		layout.loadGroups(groupNames, (int[2]){0, 1}, 2);
		layout.disableAllAnimations();
		layout.resetAnim(0);

		static const char *tbNames[] = {
			"Title", "TitleS",
			"Name", "NameS",
			"LeftName", "LeftNameS",
			"RightName", "RightNameS"
		};
		static const char *paneNames[] = {
			"TitleContainer", "NamesContainer",
			"OneNameC", "TwoNamesC",
		};

		layout.getTextBoxes(tbNames, &Title, 8);
		layout.getPanes(paneNames, &TitleContainer, 4);

		layoutLoaded = true;
	}
	return layoutLoaded;
}

bool dCreditsMgr_c::loadTitleLayout() {
	if (!titleLayoutLoaded) {
		if (!titleLayout.loadArc("StaffRollTitle.arc", false))
			return false;

		titleLayout.build("StaffRollTitle.brlyt");

		static const char *brlanNames[] = {
			"StaffRollTitle_appear1.brlan",
			"StaffRollTitle_appear2.brlan",
		};
		static const char *groupNames[] = {
			"TheOnlyGroup", "TheOnlyGroup"
		};

		titleLayout.loadAnimations(brlanNames, 2);
		titleLayout.loadGroups(groupNames, (int[2]){0, 1}, 2);
		titleLayout.disableAllAnimations();
		titleLayout.resetAnim(1);

		float propScale = 1.3f;
		if (!IsWideScreen())
			propScale *= 0.85f;

		N_proportionC_00 = titleLayout.findPaneByName("N_proportionC_00");
		N_proportionC_00->trans.y = -130.0f;
		N_proportionC_00->scale.x = propScale;
		N_proportionC_00->scale.y = propScale;

		titleLayoutLoaded = true;
	}
	return titleLayoutLoaded;
}

extern "C" dCourse_c::rail_s *GetRail(int id);

void dCreditsMgr_c::doAutoscroll(int pathID) {
	OSReport("Activating Autoscroll with path %d\n", pathID);

	getNextEFB = true;
	renderer.isEnabled = true;
	renderer.flipFrame = 0;
	renderer.scheduledBG = pathID;
	MakeMarioEnterDemoMode();

	char *autoscrInfo = ((char*)dBgGm_c::instance) + 0x900AC;
	*((u8*)(autoscrInfo + 0x14)) = pathID;
	*((u8*)(autoscrInfo + 0x15)) = 1; // unk11
	*((u8*)(autoscrInfo + 0x16)) = 0; // atEnd Related
	*((u8*)(autoscrInfo + 0x17)) = 2; // atEnd
	*((u8*)(autoscrInfo + 0x18)) = 0; // mode
	*((u8*)(autoscrInfo + 0x1A)) = 1; // isAutoscrolling
	*((u32*)(((char*)dBgGm_c::instance) + 0x900EC)) = 0; // node ID
	*((u32*)(((char*)dBgGm_c::instance) + 0x900F0)) = 0; // ?

	currentPathID = pathID;

	dCourse_c::rail_s *rail = GetRail(pathID);
	dCourse_c *course = dCourseFull_c::instance->get(GetAreaNum());

	dCourse_c::railNode_s *firstNode = &course->railNode[rail->startNode];
	dCourse_c::railNode_s *secondNode = &course->railNode[rail->startNode+1];

	fauxScrollFrame = 0;
	fauxScrollMinX = firstNode->xPos;
	fauxScrollMaxX = secondNode->xPos;
	fauxScrollY = -firstNode->yPos;

	CameraLockEnabled = 1;

	// set directions
	isLockPlayerRotation = true;
	lockedPlayerRotation = endingMode ? 0 : 0x3000;

	for (int i = 0; i < 4; i++) {
		dAcPy_c *player;
		if ((player = dAcPy_c::findByID(i))) {
			player->direction = 0;
			player->rot.y = 0x3000;
		}
	}
}

void dCreditsMgr_c::animComplete() {
	positionPlayers();
}


void dCreditsMgr_c::positionPlayers() {
	dCourse_c *course = dCourseFull_c::instance->get(GetAreaNum());
	dCourse_c::nextGoto_s *entrance = course->getNextGotoByID(currentPathID);

	float diff = endingMode ? 48.0f : 24.0f;
	float playerPositions[4];
	playerPositions[0] = entrance->xPos;
	playerPositions[1] = playerPositions[0] - diff;
	playerPositions[2] = playerPositions[0] + diff;
	playerPositions[3] = playerPositions[0] + diff + diff;

	// This is annoying
	dAcPy_c *players[4];
	for (int i = 0; i < 4; i++)
		players[i] = dAcPy_c::findByID(i);

	static const int crap[4] = {0,1,3,2};

	int whichPos = 0;
	for (int i = 0; i < 4; i++) {
		dAcPy_c *player = 0;
		// Find the player matching this ID
		for (int j = 0; j < 4; j++) {
			if (Player_ID[players[j]->settings & 0xF] == crap[i]) {
				player = players[j];
				break;
			}
		}

		if (player) {
			player->pos.x = playerPositions[whichPos];
			player->pos.y = -(entrance->yPos + 16);
			player->direction = 0;
			player->rot.y = 0x3000;
			player->speed.x = player->speed.y = player->speed.z = 0.0f;
			dPlayerModelHandler_c *pmh = (dPlayerModelHandler_c*)(((u32)player) + 0x2A60);
			pmh->mdlClass->startAnimation(0, 1.0f, 0.0f, 0.0f);
			whichPos++;

			u32 *pInactivityCounter = (u32*)(((u32)player) + 0x480);
			*pInactivityCounter = 177;

			endingLockPositions[i].x = player->pos.x;
			endingLockPositions[i].y = player->pos.y;
		}
	}
}


void dCreditsMgr_c::enableZoom() {
	BgGmBase::manualZoomEntry_s &zoom = dBgGm_c::instance->manualZooms[0];
	zoom.x1 = 10218.0f;
	zoom.x2 = 11000.0f;
	zoom.y1 = -200.0f;
	zoom.y2 = -600.0f;

	zoom.x1 = 1100.0f;
	zoom.x2 = 1300.0f;
	zoom.y1 = -400.0f;
	zoom.y2 = -550.0f;
	zoom.zoomLevel = 7;
	zoom.unkValue6 = 0;
	zoom.firstFlag = 0;
}
void dCreditsMgr_c::disableZoom() {
	BgGmBase::manualZoomEntry_s &zoom = dBgGm_c::instance->manualZooms[0];
	zoom.unkValue6 = 100;
}
void dCreditsMgr_c::playerWinAnim() {
	// who won?
	// First, get the amounts
	u8 *amountsU8 = GetStaffCreditScore() + 0x288;
	int *playerAmounts = (int*)(amountsU8);
	int maxSoFar = 0;

	for (int i = 0; i < 4; i++) {
		if (playerAmounts[i] > maxSoFar)
			maxSoFar = playerAmounts[i];
	}

	if (maxSoFar == 0)
		return;

	// did anyone win?
	for (int i = 0; i < 4; i++) {
		if (playerAmounts[i] == maxSoFar) {
			dAcPy_c *player = dAcPy_c::findByID(i);
			if (!player)
				continue;

			player->setAnimePlayWithAnimID(goal_puton_capA);
			player->setFlag(0x24);

			static const int vocs[4] = {
				SE_VOC_MA_CLEAR_NORMAL,
				SE_VOC_LU_CLEAR_NORMAL,
				SE_VOC_KO_CLEAR_NORMAL,
				SE_VOC_KO2_CLEAR_NORMAL
			};
			dPlayerModelHandler_c *pmh = (dPlayerModelHandler_c*)(((u32)player) + 0x2A60);
			int voc = vocs[pmh->mdlClass->player_id_2];
			nw4r::snd::SoundHandle handle;
			PlaySoundWithFunctionB4(SoundRelatedClass, &handle, voc, 1);

			int powerup = *((u32*)( 0x1090 + ((u8*)player) ));
			handle.SetPitch(powerup == 3 ? 1.5f : 1.0f);
		}
	}
}

void dCreditsMgr_c::playerLookUp() {
	_120 |= 8;
	lookAtMode = 2; // Higher maximum distance
}
void dCreditsMgr_c::theEnd() {
	GetTheEnd()->willShow = true;
}
void dCreditsMgr_c::exitStage() {
	SaveBlock *save = GetSaveFile()->GetBlock(-1);
	bool wasPreviouslyBeat = (save->bitfield & 2) != 0;
	save->bitfield |= 2;

	ExitStage(WORLD_MAP, wasPreviouslyBeat ? 0 : 0x20000000, BEAT_LEVEL, CIRCLE_WIPE);
}

Vec2 dCreditsMgr_c::_vf70() {
	// HACK OF THE MILLENIUM
	// DON'T TRY THIS AT HOME.
	Vec2 *v = (Vec2*)this;
	v->x = 10454.0f;
	v->y = -320.0f;
	return (const Vec2){12345.0f, 67890.f};
}



void EFBMagic2() {
	if (getNextEFB) {
		getNextEFB = false;

		GXRModeObj *ro = nw4r::g3d::G3DState::GetRenderModeObj();
		efbTexture.format = GX_TF_RGB565;
		efbTexture.width = ro->fbWidth;
		efbTexture.height = ro->efbHeight;
		efbTexture.wrapS = GX_CLAMP;
		efbTexture.wrapT = GX_CLAMP;

		if (efbTexture.getBuffer() == 0)
			efbTexture.allocateBuffer(GameHeaps[2]);

		GXSetTexCopySrc(0, 0, efbTexture.width, efbTexture.height);
		GXSetTexCopyDst(efbTexture.width, efbTexture.height, efbTexture.format, GX_FALSE);
		GXSetCopyFilter(GX_FALSE, 0, GX_FALSE, 0);
		GXCopyTex(efbTexture.getBuffer(), GX_FALSE);

		GXPixModeSync();
		GXInvalidateTexAll();
	}
}

void dFlipbookRenderer_c::execute() {
	if (flipFrame == 7) {
		loadNewBG(scheduledBG, true);
	}

	if (isEnabled) {
		flipFrame += 7;

		ClassWithCameraInfo *cwci = ClassWithCameraInfo::instance;
		if (flipFrame > int(cwci->screenWidth)) {
			loadNewBG(scheduledBG, false);
			dCreditsMgr_c::instance->animComplete();

			if (!dCreditsMgr_c::instance->endingMode)
				MakeMarioExitDemoMode();
			isEnabled = false;
			OSReport("DONE!\n");
		}
	}
}


static void setupGXForDrawingCrap() {
	GXSetNumChans(0);
	GXSetChanCtrl(GX_COLOR0A0, GX_DISABLE, GX_SRC_REG, GX_SRC_REG, GX_LIGHTNULL, GX_DF_NONE, GX_AF_NONE);
	GXSetChanAmbColor(GX_COLOR0A0, (GXColor){255,255,255,255});
	GXSetChanMatColor(GX_COLOR0A0, (GXColor){255,255,255,255});
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX3x4, GX_TG_NRM, GX_IDENTITY, GX_FALSE, GX_DTTIDENTITY);

	GXSetNumTevStages(1);
	GXSetNumIndStages(0);
	for (int i = 0; i < 0x10; i++)
		GXSetTevDirect(i);

	GXSetTevOp(GX_TEVSTAGE0, GX_REPLACE);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLORNULL);

	GXSetTevSwapModeTable(GX_TEV_SWAP0, GX_CH_RED, GX_CH_GREEN, GX_CH_BLUE, GX_CH_ALPHA);

	GXSetZCompLoc(GX_FALSE);
	GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_SET);
	//GXSetBlendMode(GX_BM_NONE, GX_BL_ZERO, GX_BL_ZERO, GX_LO_SET);
	GXSetZMode(GX_TRUE, GX_ALWAYS, GX_FALSE);
	GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_OR, GX_ALWAYS, 0);

	GXSetFog(GX_FOG_NONE, 0, 0, 0, 0, (GXColor){0,0,0,0});
	GXSetFogRangeAdj(GX_FALSE, 0, 0);

	GXSetAlphaUpdate(GX_TRUE);

	GXSetCullMode(GX_CULL_NONE);

	GXSetDither(GX_TRUE);

	GXSetTevColor(GX_TEVREG0, (GXColor){255,255,255,255});
	GXSetTevColor(GX_TEVREG1, (GXColor){255,255,255,255});
	GXSetTevColor(GX_TEVREG2, (GXColor){255,255,255,255});

	GXSetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);

	nw4r::g3d::Camera cam(GetCameraByID(GetCurrentCameraID()));
	Mtx matrix;
	cam.GetCameraMtx(&matrix);
	GXLoadPosMtxImm(matrix, 0);
	GXSetCurrentMtx(0);

	GXClearVtxDesc();

	GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
	GXSetVtxDesc(GX_VA_NRM, GX_DIRECT);

	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM, GX_NRM_XYZ, GX_F32, 0);
}

void dFlipbookRenderer_c::drawXlu() {
	if (!isEnabled || flipFrame < 2)
		return;

	setupGXForDrawingCrap();

	ClassWithCameraInfo *cwci = ClassWithCameraInfo::instance;

	float screenTop = cwci->screenTop + Stage80::instance->screenYOffset;
	float left = cwci->screenLeft + Stage80::instance->screenXOffset;
	float right = left + cwci->screenWidth;

	float halfWidth = (cwci->screenWidth * 0.5f);
	float midpoint = left + halfWidth;

	float value = (flipFrame / cwci->screenWidth);
	float sinThing = 50.0f * sin(3.14159f * value);

	bool drawBackside = (flipFrame > halfWidth);

	float xformedFlipEdge = right - flipFrame;

	// EFB SPECIFIC VERTICAL VARIABLES
	float efbEffectiveHeight = cwci->screenHeight;
	if (!IsWideScreen())
		efbEffectiveHeight *= 1.295f;

	float efbHalfHeight = (efbEffectiveHeight * 0.5f);

	float efbYCentre = screenTop - (cwci->screenHeight * 0.5f);
	if (!IsWideScreen())
		efbYCentre += 1.0f;
	float efbTop = efbYCentre + efbHalfHeight;
	float efbBottom = efbYCentre - efbHalfHeight;

	float efbExCoord = (efbEffectiveHeight + sinThing + sinThing) * 0.5f;
	// TPL SPECIFIC VERTICAL VARIABLES
	float tplHalfHeight = cwci->screenHeight * 0.5f;
	float tplTop = screenTop;
	float tplBottom = screenTop - cwci->screenHeight;

	float tplExCoord = (cwci->screenHeight + sinThing + sinThing) * 0.5f;


	// EFB Left
	efbTexture.load(GX_TEXMAP0);

	GXBegin(GX_QUADS, GX_VTXFMT0, 4);
	{
		// Bottom Right
		GXPosition3f32(midpoint, efbBottom, 9990.0f);
		GXNormal3f32(0.5f, 1.0f, 1.0f);
		// Top Right
		GXPosition3f32(midpoint, efbTop, 9990.0f);
		GXNormal3f32(0.5f, 0.0f, 1.0f);
		// Top Left
		GXPosition3f32(left, efbTop, 9990.0f);
		GXNormal3f32(0.0f, 0.0f, 1.0f);
		// Bottom Left
		GXPosition3f32(left, efbBottom, 9990.0f);
		GXNormal3f32(0.0f, 1.0f, 1.0f);
	}
	GXEnd();

	// TPL Right
	GXLoadTexObj(&bgTexObj[1], GX_TEXMAP0);

	GXBegin(GX_QUADS, GX_VTXFMT0, 4);
	{
		// Bottom Right
		GXPosition3f32(right, tplBottom, 9990.0f);
		GXNormal3f32(1.0f, 1.0f, 1.0f);
		// Top Right
		GXPosition3f32(right, tplTop, 9990.0f);
		GXNormal3f32(1.0f, 0.0f, 1.0f);
		// Top Left
		GXPosition3f32(midpoint, tplTop, 9990.0f);
		GXNormal3f32(0.5f, 0.0f, 1.0f);
		// Bottom Left
		GXPosition3f32(midpoint, tplBottom, 9990.0f);
		GXNormal3f32(0.5f, 1.0f, 1.0f);

	}
	GXEnd();


	if (!drawBackside) {
		// Flipping right side: EFB

		efbTexture.load(GX_TEXMAP0);

		GXBegin(GX_QUADS, GX_VTXFMT0, 4);

		// EFB Right (Flipped)
		// Bottom Left
		GXPosition3f32(midpoint, efbBottom, 9990.0f);
		GXNormal3f32(efbHalfHeight * 0.5f, efbHalfHeight, efbHalfHeight);
		// Top Left
		GXPosition3f32(midpoint, efbTop, 9990.0f);
		GXNormal3f32(efbHalfHeight * 0.5f, 0.0f, efbHalfHeight);
		// Top Right
		GXPosition3f32(xformedFlipEdge, efbTop + sinThing, 9990.0f);
		GXNormal3f32(efbExCoord, 0.0f, efbExCoord);
		// Bottom Right
		GXPosition3f32(xformedFlipEdge, efbBottom - sinThing, 9990.0f);
		GXNormal3f32(efbExCoord, efbExCoord, efbExCoord);

		GXEnd();

	} else {
		// Flipping left side

		GXLoadTexObj(&bgTexObj[1], GX_TEXMAP0);

		GXBegin(GX_QUADS, GX_VTXFMT0, 4);

		// TPL Left (Flipped))
		// Bottom Left
		GXPosition3f32(xformedFlipEdge, tplBottom - sinThing, 9990.0f);
		GXNormal3f32(0.0f, tplExCoord, tplExCoord);
		// Top Left
		GXPosition3f32(xformedFlipEdge, tplTop + sinThing, 9990.0f);
		GXNormal3f32(0.0f, 0.0f, tplExCoord);
		// Top Right
		GXPosition3f32(midpoint, tplTop, 9990.0f);
		GXNormal3f32(tplHalfHeight * 0.5f, 0.0f, tplHalfHeight);
		// Bottom Right
		GXPosition3f32(midpoint, tplBottom, 9990.0f);
		GXNormal3f32(tplHalfHeight * 0.5f, tplHalfHeight, tplHalfHeight);

		GXEnd();
	}
}


void dFlipbookRenderer_c::drawOpa() {
	setupGXForDrawingCrap();

	GXLoadTexObj(&bgTexObj[0], GX_TEXMAP0);

	ClassWithCameraInfo *cwci = ClassWithCameraInfo::instance;
	float left = cwci->screenLeft + Stage80::instance->screenXOffset;
	float right = left + cwci->screenWidth;
	float top = cwci->screenTop + Stage80::instance->screenYOffset;
	float bottom = top - cwci->screenHeight;

	GXBegin(GX_QUADS, GX_VTXFMT0, 4);
	GXPosition3f32(right, bottom, -4000.0f);
	GXNormal3f32(1.0f, 1.0f, 1.0f);
	GXPosition3f32(right, top, -4000.0f);
	GXNormal3f32(1.0f, 0.0f, 1.0f);
	GXPosition3f32(left, top, -4000.0f);
	GXNormal3f32(0.0f, 0.0f, 1.0f);
	GXPosition3f32(left, bottom, -4000.0f);
	GXNormal3f32(0.0f, 1.0f, 1.0f);
	GXEnd();
}

void dFlipbookRenderer_c::loadNewBG(int bgID, bool isBackface) {
	OSReport("Will load BG: %d\n", bgID);

	int setID = isBackface ? 1 : 0;

	char bgName[32];
	sprintf(bgName, isBackface ? "/Back%d.tpl.LZ" : "/BG%d.tpl.LZ", bgID);
	OSReport("Getting %s\n", bgName);

	u8 *sourceBuf = getResource("CreditsBG", bgName);
	u32 bufSize = CXGetUncompressedSize(sourceBuf);

	if (tplBuffer[setID] && (tplBufferSize[setID] != bufSize)) {
		OSReport("Current TPL buffer (%p) is size %d (0x%x), freeing\n", tplBuffer[setID], tplBufferSize[setID], tplBufferSize[setID]);
		EGG__Heap__free(tplBuffer[setID], GameHeaps[2]);
		tplBuffer[setID] = 0;
	}

	if (!tplBuffer[setID]) {
		OSReport("Allocating TPL buffer of size %d (0x%x)\n", bufSize, bufSize);
		tplBuffer[setID] = (char*)EGG__Heap__alloc(bufSize, 0x20, GameHeaps[2]);
		tplBufferSize[setID] = bufSize;
	}

	CXUncompressLZ(sourceBuf, tplBuffer[setID]);
	OSReport("Butts. Decompressing %p to %p.\n", sourceBuf, tplBuffer[setID]);

	TPLBind((TPLPalette*)tplBuffer[setID]);
	TPLImage *image = TPLGet((TPLPalette*)tplBuffer[setID], 0);
	TPLTexHeader *tex = image->texture;
	OSReport("Received TPLHeader %p; Data: %p; Size: %d x %d; Format; %d\n", tex, tex->data, tex->width, tex->height, tex->format);

	GXInitTexObj(&bgTexObj[setID], tex->data, tex->width, tex->height,
			tex->format, tex->wrapS, tex->wrapT, GX_FALSE);
}

dFlipbookRenderer_c::dFlipbookRenderer_c() {
	scheduledBG = -1;
}

dFlipbookRenderer_c::~dFlipbookRenderer_c() {
	for (int setID = 0; setID < 2; setID++) {
		if (tplBuffer[setID]) {
			EGG__Heap__free(tplBuffer[setID], GameHeaps[2]);
			tplBuffer[setID] = 0;
		}
	}
}



extern "C" void replayRecord();

void LoadDanceValues() {
	/*
	//OSReport("AnmSpd: %4d / DBAG: 0x%02x / Bahp: 0x%02x / Cred: 0x%02x\n",
	//	DanceValues_AnimSpeed, DanceValues_DummyBlockAndGrass, DanceValues_Bahps, DanceValues_CreditsControl);
	if (DanceValues_CreditsControl > 0)
		OSReport("[ORIG DANCE] Credits Control: 0x%02x\n", DanceValues_CreditsControl);
	// if (DanceValues_DummyBlockAndGrass > 0)
	// 	OSReport("[ORIG DANCE] DummyBlockAndGrass: 0x%02x\n", DanceValues_DummyBlockAndGrass);
	if (DanceValues_Bahps > 0)
		OSReport("[ORIG DANCE] Bahps: 0x%02x\n", DanceValues_Bahps);
	*/

	dCreditsMgr_c *cred = dCreditsMgr_c::instance;

	if (!cred) {
		replayRecord();
		return;
	}
	danceInfo_s *cmd = cred->danceCommand;
	if (!cmd)
		return;
	//OSReport("TIMER: %d\n", cred->danceTimer);

	if (cred->danceTimer == cmd->when) {
		//OSReport("Timer reached %d, triggering dance 0x%02x, next is at %d\n", cmd->when, cmd->bahpFlag, cmd[1].when);
		DanceValues_AnimSpeed = cmd->animSpeed;
		DanceValues_DummyBlockAndGrass = cmd->dummyBlockFlag;
		DanceValues_Bahps = cmd->bahpFlag;
		DanceValues_CreditsControl = cmd->creditsFlag;

		cred->danceCommand++;
	} else {
		DanceValues_AnimSpeed = 120;
		DanceValues_DummyBlockAndGrass = 0;
		DanceValues_Bahps = 0;
		DanceValues_CreditsControl = 0;
	}
}



//
// processed\../src/endingMgr.cpp
//

#include <game.h>
#include <playerAnim.h>
#include <sfx.h>
#include <stage.h>
extern void *SoundRelatedClass;

class dEndingMgr_c : public daBossDemo_c {
	int onCreate();
	int onDelete();

	void init();
	Vec2 _vf70();

	USING_STATES(dEndingMgr_c);
	DECLARE_STATE_VIRTUAL(DemoSt);
	DECLARE_STATE(GoRight);
	DECLARE_STATE(LookUp);
	DECLARE_STATE(JumpOntoSwitch);
	DECLARE_STATE(ThanksPeach);

	dAcPy_c *players[4];

	int timer;

	static dEndingMgr_c *build();
};

dEndingMgr_c *dEndingMgr_c::build() {
	void *buf = AllocFromGameHeap1(sizeof(dEndingMgr_c));
	return new(buf) dEndingMgr_c;
}


int dEndingMgr_c::onCreate() {
	if (StageE4::instance->currentBossDemo) {
		fBase_c::Delete();
		return false;
	}
	StageE4::instance->currentBossDemo = this;

	init();

	acState.setState(&StateID_DemoSt);
	acState.executeNextStateThisTick();

	return true;
}

int dEndingMgr_c::onDelete() {
	nw4r::snd::SoundHandle *fanfare = (nw4r::snd::SoundHandle*)
		(((u32)SoundRelatedClass) + 0x900);
	if (fanfare->Exists())
		fanfare->Stop(60);

	daBossDemo_c::onDelete();
	WLClass::instance->disableDemoControl(true);
	return true;
}


void dEndingMgr_c::init() {
	dStageActorMgr_c::instance->_BCA = true;
	WLClass::instance->demoControlAllPlayers();
	BalloonRelatedClass::instance->_20 = 1;
}


CREATE_STATE(dEndingMgr_c, DemoSt);

void dEndingMgr_c::beginState_DemoSt() {
	_360 = 1;
}
void dEndingMgr_c::endState_DemoSt() {
}
void dEndingMgr_c::executeState_DemoSt() {
	for (int i = 0; i < 4; i++) {
		dAcPy_c *player;
		if (player = dAcPy_c::findByID(i)) {
			if (!player->isReadyForDemoControlAction())
				return;
		}
	}
	acState.setState(&StateID_GoRight);
}

static float manipFourPlayerPos(int id, float pos) {
	int fromRight = 3 - id;
	return pos - (fromRight * 20.0f);
}

CREATE_STATE(dEndingMgr_c, GoRight);
void dEndingMgr_c::beginState_GoRight() {
	timer = 0;

	// Sort the players into a list
	// We shall first find the rightmost player, then the second
	// rightmost, then ...
	float maxBound = 50000.0f;
	for (int targetOffs = 3; targetOffs >= 0; targetOffs--) {
		float maxX = 0.0f;
		dAcPy_c *maxPlayer = 0;

		for (int check = 0; check < 4; check++) {
			if (dAcPy_c *player = dAcPy_c::findByID(check)) {
				if (player->pos.x >= maxBound)
					continue;

				if (player->pos.x > maxX) {
					maxX = player->pos.x;
					maxPlayer = player;
				}
			}
		}

		maxBound = maxX;
		players[targetOffs] = maxPlayer;
	}

	// And now move them
	for (int i = 0; i < 4; i++) {
		if (dAcPy_c *player = players[i]) {
			float target = manipFourPlayerPos(i, 1060.0f);
			float speed = 2.0f;
			player->moveInDirection(&target, &speed);
		}
	}
}

void dEndingMgr_c::endState_GoRight() { }

void dEndingMgr_c::executeState_GoRight() {
	for (int i = 0; i < 4; i++) {
		if (dAcPy_c *player = players[i]) {
			if (player->pos.x > manipFourPlayerPos(i, 920.0f)) {
				player->demoMoveSpeed *= 0.994f;
			}
		}
	}

	// Can we leave this state yet?
	for (int i = 0; i < 4; i++) {
		if (dAcPy_c *player = players[i]) {
			if (!player->isReadyForDemoControlAction())
				return;
		}
	}

	// WE CAN LEAVE YAY
	timer++;
	if (timer >= 20)
		acState.setState(&StateID_LookUp);
}

CREATE_STATE(dEndingMgr_c, LookUp);
void dEndingMgr_c::beginState_LookUp() {
	_120 |= 8;
	lookAtMode = 2; // Higher maximum distance

	timer = 0;
}

void dEndingMgr_c::endState_LookUp() {
}

void dEndingMgr_c::executeState_LookUp() {
	timer++;

	if (timer >= 60)
		_120 &= ~8;

	if (timer >= 90)
		acState.setState(&StateID_JumpOntoSwitch);
}


CREATE_STATE(dEndingMgr_c, JumpOntoSwitch);
void dEndingMgr_c::beginState_JumpOntoSwitch() {
	for (int i = 0; i < 4; i++) {
		if (dAcPy_c *player = players[i]) {
			float target = manipFourPlayerPos(i, 1144.0f);
			float speed = 2.0f;
			player->moveInDirection(&target, &speed);
		}
	}

	timer = 0;
}
void dEndingMgr_c::endState_JumpOntoSwitch() {
}
void dEndingMgr_c::executeState_JumpOntoSwitch() {
	dAcPy_c *player = players[3];

	if (timer > 60) {
		acState.setState(&StateID_ThanksPeach);
	} else if (player->isReadyForDemoControlAction()) {
		player->input.setTransientForcedButtons(WPAD_DOWN);
	} else if (timer > 30) {
		player->input.unsetPermanentForcedButtons(WPAD_TWO);
	} else if (timer > 15) {
		player->input.setPermanentForcedButtons(WPAD_TWO);
	}

	timer++;
}

static daEnBossKoopaDemoPeach_c *getPeach() {
	return (daEnBossKoopaDemoPeach_c*)dEn_c::search(EN_BOSS_KOOPA_DEMO_PEACH);
}

CREATE_STATE(dEndingMgr_c, ThanksPeach);
void dEndingMgr_c::beginState_ThanksPeach() {
	timer = -1;
	getPeach()->doStateChange(&daEnBossKoopaDemoPeach_c::StateID_Turn);
}
void dEndingMgr_c::endState_ThanksPeach() {
}
void dEndingMgr_c::executeState_ThanksPeach() {
	daEnBossKoopaDemoPeach_c *peach = getPeach();
	dStateBase_c *peachSt = peach->acState.getCurrentState();

	if (peach->stage == 1 && peachSt == &peach->StateID_Turn) {
		timer++;
		if (timer == 1) {
			// This plays the Peach harp fanfare
			sub_8019C390(_8042A788, 6);
		}
		if (timer > 20) {
			peach->doStateChange(&peach->StateID_Open);
			peach->_120 |= 8;
			timer = -1;
		}
	}

	if (peach->stage == 1 && peachSt == &peach->StateID_Open) {
		peach->doStateChange(&peach->StateID_Rescue);
	}

	if (peach->stage == 3 && peachSt == &peach->StateID_Rescue) {
		peach->doStateChange(&peach->StateID_Thank);
	}

	if (peachSt == &peach->StateID_Thank) {
		// 1. Freeze Peach by changing to Stage 8 as soon as she's almost done
		// 2. Do our thing
		// 3. Go back!
		if (peach->stage == 0 && peach->counter == 33 && timer == -1) {
			peach->stage = 8;
			timer = 0;
		} else if (peach->stage == 8) {
			timer++;
			if (timer == 8) {
				float target = peach->pos.x - 20.0f;
				float speed = 1.5f;
				players[3]->moveInDirection(&target, &speed);
			} else if (timer == 90) {
				peach->stage = 0;
				players[3]->setAnimePlayWithAnimID(dm_escort);
			}
		} else if (peach->stage == 7) {
			// All the players are glad
			timer = 0;
			peach->stage = 9;
			for (int i = 0; i < 4; i++) {
				if (dAcPy_c *player = players[i]) {
					OSReport("Going to dmglad\n");
					OSReport("Player %d states: %s and %s\n", i, player->states2.getCurrentState()->getName(), player->demoStates.getCurrentState()->getName());
					player->setAnimePlayWithAnimID(dm_glad);

					static const int vocs[4] = {
						SE_VOC_MA_SAVE_PRINCESS,
						SE_VOC_LU_SAVE_PRINCESS,
						SE_VOC_KO_SAVE_PRINCESS,
						SE_VOC_KO2_SAVE_PRINCESS
					};
					dPlayerModelHandler_c *pmh = (dPlayerModelHandler_c*)(((u32)player) + 0x2A60);
					int voc = vocs[pmh->mdlClass->player_id_2];
					nw4r::snd::SoundHandle handle;
					PlaySoundWithFunctionB4(SoundRelatedClass, &handle, voc, 1);

					int powerup = *((u32*)( 0x1090 + ((u8*)player) ));
					handle.SetPitch(powerup == 3 ? 1.5f : 1.0f);
				}
			}
		} else if (peach->stage == 9) {
			timer++;
			if (timer == 90) {
				RESTART_CRSIN_LevelStartStruct.purpose = 0;
				RESTART_CRSIN_LevelStartStruct.world1 = 6;
				RESTART_CRSIN_LevelStartStruct.world2 = 6;
				RESTART_CRSIN_LevelStartStruct.level1 = 40;
				RESTART_CRSIN_LevelStartStruct.level2 = 40;
				RESTART_CRSIN_LevelStartStruct.areaMaybe = 0;
				RESTART_CRSIN_LevelStartStruct.entrance = 0xFF;
				RESTART_CRSIN_LevelStartStruct.unk4 = 0; // load replay
				DontShowPreGame = true;
				ExitStage(RESTART_CRSIN, 0, BEAT_LEVEL, MARIO_WIPE);
			}
		}
	}
}




Vec2 dEndingMgr_c::_vf70() {
	// HACK OF THE MILLENIUM
	// DON'T TRY THIS AT HOME.
	Vec2 *v = (Vec2*)this;
	v->x = 1195.0f;
	v->y = -394.0f;
	return (const Vec2){12345.0f, 67890.f};
}


//
// processed\../src/chestnut.cpp
//

#include <game.h>
#include <sfx.h>
const char *ChestnutFileList[] = {"chestnut", 0};

class daEnChestnut_c : public dEn_c {
	public:
		static daEnChestnut_c *build();

		mHeapAllocator_c allocator;
		nw4r::g3d::ResFile resFile;
		m3d::mdl_c model;
		m3d::anmChr_c animation;

		void playAnimation(const char *name, bool loop = false);
		void playLoopedAnimation(const char *name) {
			playAnimation(name, true);
		}

		int objNumber;
		int starCoinNumber;
		bool ignorePlayers;
		bool breaksBlocks;
		float shakeWindow, fallWindow;

		int timeSpentExploding;

		lineSensor_s belowSensor;

		float nearestPlayerDistance();

		int onCreate();
		int onDelete();
		int onExecute();
		int onDraw();

		void spawnObject();

		bool collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther);
		bool collisionCat5_Mario(ActivePhysics *apThis, ActivePhysics *apOther);

		bool CreateIceActors();

		u32 canBePowed();
		void powBlockActivated(bool isNotMPGP);

		USING_STATES(daEnChestnut_c);
		DECLARE_STATE(Idle);
		DECLARE_STATE(Shake);
		DECLARE_STATE(Fall);
		DECLARE_STATE(Explode);
};

CREATE_STATE(daEnChestnut_c, Idle);
CREATE_STATE(daEnChestnut_c, Shake);
CREATE_STATE(daEnChestnut_c, Fall);
CREATE_STATE(daEnChestnut_c, Explode);

daEnChestnut_c *daEnChestnut_c::build() {
	void *buf = AllocFromGameHeap1(sizeof(daEnChestnut_c));
	return new(buf) daEnChestnut_c;
}


int daEnChestnut_c::onCreate() {
	// Get settings
	int texNum = settings & 0xF;
	int rawScale = (settings & 0xF0) >> 4;
	starCoinNumber = (settings & 0xF00) >> 8;
	ignorePlayers = ((settings & 0x1000) != 0);
	breaksBlocks = ((settings & 0x2000) != 0);
	objNumber = (settings & 0xF0000) >> 16;

	if ((settings & 0x4000) != 0) {
		shakeWindow = 96.0f;
		fallWindow = 64.0f;
	} else {
		shakeWindow = 64.0f;
		fallWindow = 32.0f;
	}

	// Set up models
	allocator.link(-1, GameHeaps[0], 0, 0x20);

	char rfName[64];
	sprintf(rfName, "g3d/t%02d.brres", texNum);

	resFile.data = getResource("chestnut", rfName);

	nw4r::g3d::ResMdl resMdl = resFile.GetResMdl("kuribo_iga");
	nw4r::g3d::ResAnmChr resAnm = resFile.GetResAnmChr("wait");

	model.setup(resMdl, &allocator, 0x224, 1, 0);
	SetupTextures_Enemy(&model, 0);

	animation.setup(resMdl, resAnm, &allocator, 0);

	allocator.unlink();


	// Scale us
	scale.x = scale.y = scale.z = (1.0f + (float(rawScale) * 0.5f));

	// Physics and crap
	ActivePhysics::Info ccInfo;
	ccInfo.xDistToCenter = 0.0f;
	ccInfo.xDistToEdge = 12.0f * scale.x;
	ccInfo.yDistToCenter = 1.0f + (12.0f * scale.y);
	ccInfo.yDistToEdge = 12.0f * scale.y;

	ccInfo.category1 = 3;
	ccInfo.category2 = 0;
	ccInfo.bitfield1 = 0x6F;
	ccInfo.bitfield2 = 0xFFBAFFFE;

	ccInfo.unkShort1C = 0;
	ccInfo.callback = &dEn_c::collisionCallback;

	aPhysics.initWithStruct(this, &ccInfo);
	aPhysics.addToList();

	// WE'RE READY
	doStateChange(&StateID_Idle);

	return true;
}

void daEnChestnut_c::playAnimation(const char *name, bool loop) {
	animation.bind(&model, resFile.GetResAnmChr(name), !loop);
	model.bindAnim(&animation, 0.0f);
	animation.setUpdateRate(1.0f);
}

int daEnChestnut_c::onDelete() {
	aPhysics.removeFromList();
	return true;
}

int daEnChestnut_c::onExecute() {
	acState.execute();

	matrix.translation(pos.x, pos.y, pos.z);

	model.setDrawMatrix(matrix);
	model.setScale(&scale);
	model.calcWorld(false);

	model._vf1C();

	return true;
}

int daEnChestnut_c::onDraw() {
	model.scheduleForDrawing();

	return true;
}

float daEnChestnut_c::nearestPlayerDistance() {
	float bestSoFar = 10000.0f;

	for (int i = 0; i < 4; i++) {
		if (dAcPy_c *player = dAcPy_c::findByID(i)) {
			if (strcmp(player->states2.getCurrentState()->getName(), "dAcPy_c::StateID_Balloon")) {
				float thisDist = abs(player->pos.x - pos.x);
				if (thisDist < bestSoFar)
					bestSoFar = thisDist;
			}
		}
	}

	return bestSoFar;
}



void daEnChestnut_c::beginState_Idle() {
	playLoopedAnimation("wait");
}
void daEnChestnut_c::endState_Idle() { }


void daEnChestnut_c::executeState_Idle() {
	if (ignorePlayers)
		return;

	float dist = nearestPlayerDistance();

	if (dist < fallWindow)
		doStateChange(&StateID_Fall);
	else if (dist < shakeWindow)
		doStateChange(&StateID_Shake);
}



void daEnChestnut_c::beginState_Shake() {
	playLoopedAnimation("shake");
	animation.setUpdateRate(2.0f);

	nw4r::snd::SoundHandle handle;
	PlaySoundWithFunctionB4(SoundRelatedClass, &handle, SE_PLY_CLIMB_KUSARI, 1);
}
void daEnChestnut_c::endState_Shake() { }

void daEnChestnut_c::executeState_Shake() {
	float dist = nearestPlayerDistance();

	if (dist >= shakeWindow)
		doStateChange(&StateID_Idle);
	else if (dist < fallWindow)
		doStateChange(&StateID_Fall);
}



void daEnChestnut_c::beginState_Fall() {
	animation.setUpdateRate(0.0f); // stop animation

	int size = 12*scale.x;

	belowSensor.flags = SENSOR_LINE;
	if (breaksBlocks)
		belowSensor.flags |= SENSOR_10000000 | SENSOR_BREAK_BLOCK | SENSOR_BREAK_BRICK;
	// 10000000 makes it pass through bricks

	belowSensor.lineA = -size << 12;
	belowSensor.lineB = size << 12;
	belowSensor.distanceFromCenter = 0;

	collMgr.init(this, &belowSensor, 0, 0);

	speed.y = 0.0f;
	y_speed_inc = -0.1875f;
	max_speed.y = -4.0f;

	nw4r::snd::SoundHandle handle;
	PlaySoundWithFunctionB4(SoundRelatedClass, &handle, SE_DEMO_OP_PRESENT_THROW_2308f, 1);
}

void daEnChestnut_c::endState_Fall() { }

void daEnChestnut_c::executeState_Fall() {
	HandleYSpeed();
	doSpriteMovement();
	UpdateObjectPosBasedOnSpeedValuesReal();

	if (collMgr.calculateBelowCollision() & (~0x400000)) {
		doStateChange(&StateID_Explode);
	}
}



void daEnChestnut_c::beginState_Explode() {
	OSReport("Entering Explode\n");
	playAnimation("break");
	animation.setUpdateRate(2.0f);

	nw4r::snd::SoundHandle handle;
	PlaySoundWithFunctionB4(SoundRelatedClass, &handle, SE_DEMO_OP_LAND_JR_0983f, 1);

	timeSpentExploding = 0;
}
void daEnChestnut_c::endState_Explode() { }

void daEnChestnut_c::executeState_Explode() {
	timeSpentExploding++;

	if (timeSpentExploding == 10) {
		S16Vec efRot = {0,0,0};
		SpawnEffect("Wm_en_burst_ss", 0, &pos, &efRot, &scale);
		spawnObject();
	}

	if (animation.isAnimationDone()) {
		Delete(1);
	}
}



bool daEnChestnut_c::CreateIceActors() {
	animation.setUpdateRate(0.0f);
	
	IceActorSpawnInfo info;
	info.flags = 0;
	info.pos = pos;
	info.pos.y -= (6.0f * info.scale.y);
	info.scale.x = info.scale.y = info.scale.z = scale.x * 1.35f;
	for (int i = 0; i < 8; i++)
		info.what[0] = 0.0f;

	return frzMgr.Create_ICEACTORs(&info, 1);
}

u32 daEnChestnut_c::canBePowed() {
	return true;
}
void daEnChestnut_c::powBlockActivated(bool isNotMPGP) {
	if (!isNotMPGP)
		return;

	dStateBase_c *state = acState.getCurrentState();
	if (state == &StateID_Idle || state == &StateID_Shake)
		doStateChange(&StateID_Fall);
}

bool daEnChestnut_c::collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther) {
	SpawnEffect("Wm_en_igafirehit", 0, &pos, &rot, &scale);

	if (acState.getCurrentState() != &StateID_Explode)
		doStateChange(&StateID_Explode);

	return true;
}

bool daEnChestnut_c::collisionCat5_Mario(ActivePhysics *apThis, ActivePhysics *apOther) {
	this->_vf220(apOther->owner);
	return true;
}


void daEnChestnut_c::spawnObject() {
	VEC3 acPos = pos;

	static const u32 things[] = {
		EN_KURIBO, 0,
		EN_TOGEZO, 0,
		EN_COIN_JUMP, 0,
		EN_ITEM, 0x05000009,
		EN_STAR_COIN, 0x10000000,
	};

	u32 acSettings = things[objNumber*2+1];

	if (objNumber == 4) {
		acSettings |= (starCoinNumber << 8);
		acPos.x -= 12.0f;
		acPos.y += 32.0f;
	}

	aPhysics.removeFromList();

	OSReport("Crap %d, %d, %08x\n", objNumber, things[objNumber*2], acSettings);
	dStageActor_c *ac =
		dStageActor_c::create((Actors)things[objNumber*2], acSettings, &acPos, 0, currentLayerID);

	S16Vec efRot = {0,0,0};
	SpawnEffect("Wm_ob_itemsndlandsmk", 0, &pos, &efRot, &scale);

	if (objNumber == 0) {
		dEn_c *en = (dEn_c*)ac;
		en->direction = 1;
		en->rot.y = -8192;
	}
}

//
// processed\../src/flipblock.cpp
//

#include <common.h>
#include <game.h>

const char *FlipBlockFileList[] = {"block_rotate", 0};

class daEnFlipBlock_c : public daEnBlockMain_c {
public:
	Physics::Info physicsInfo;

	int onCreate();
	int onDelete();
	int onExecute();
	int onDraw();

	void calledWhenUpMoveExecutes();
	void calledWhenDownMoveExecutes();

	void blockWasHit(bool isDown);

	bool playerOverlaps();

	mHeapAllocator_c allocator;
	nw4r::g3d::ResFile resFile;
	m3d::mdl_c model;

	int flipsRemaining;

	USING_STATES(daEnFlipBlock_c);
	DECLARE_STATE(Wait);
	DECLARE_STATE(Flipping);

	static daEnFlipBlock_c *build();
};


CREATE_STATE(daEnFlipBlock_c, Wait);
CREATE_STATE(daEnFlipBlock_c, Flipping);


int daEnFlipBlock_c::onCreate() {
	allocator.link(-1, GameHeaps[0], 0, 0x20);

	resFile.data = getResource("block_rotate", "g3d/block_rotate.brres");
	model.setup(resFile.GetResMdl("block_rotate"), &allocator, 0, 1, 0);
	SetupTextures_MapObj(&model, 0);

	allocator.unlink();



	blockInit(pos.y);

	physicsInfo.x1 = -8;
	physicsInfo.y1 = 8;
	physicsInfo.x2 = 8;
	physicsInfo.y2 = -8;

	physicsInfo.otherCallback1 = &daEnBlockMain_c::OPhysicsCallback1;
	physicsInfo.otherCallback2 = &daEnBlockMain_c::OPhysicsCallback2;
	physicsInfo.otherCallback3 = &daEnBlockMain_c::OPhysicsCallback3;

	physics.setup(this, &physicsInfo, 3, currentLayerID);
	physics.flagsMaybe = 0x260;
	physics.callback1 = &daEnBlockMain_c::PhysicsCallback1;
	physics.callback2 = &daEnBlockMain_c::PhysicsCallback2;
	physics.callback3 = &daEnBlockMain_c::PhysicsCallback3;
	physics.addToList();

	doStateChange(&daEnFlipBlock_c::StateID_Wait);

	return true;
}


int daEnFlipBlock_c::onDelete() {
	physics.removeFromList();

	return true;
}


int daEnFlipBlock_c::onExecute() {
	acState.execute();
	physics.update();
	blockUpdate();

	// now check zone bounds based on state
	if (acState.getCurrentState()->isEqual(&StateID_Wait)) {
		checkZoneBoundaries(0);
	}

	return true;
}


int daEnFlipBlock_c::onDraw() {
	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	model.setDrawMatrix(matrix);
	model.setScale(&scale);
	model.calcWorld(false);
	model.scheduleForDrawing();

	return true;
}


daEnFlipBlock_c *daEnFlipBlock_c::build() {

	void *buffer = AllocFromGameHeap1(sizeof(daEnFlipBlock_c));
	daEnFlipBlock_c *c = new(buffer) daEnFlipBlock_c;


	return c;
}


void daEnFlipBlock_c::blockWasHit(bool isDown) {
	pos.y = initialY;

	doStateChange(&StateID_Flipping);
}



void daEnFlipBlock_c::calledWhenUpMoveExecutes() {
	if (initialY >= pos.y)
		blockWasHit(false);
}

void daEnFlipBlock_c::calledWhenDownMoveExecutes() {
	if (initialY <= pos.y)
		blockWasHit(true);
}



void daEnFlipBlock_c::beginState_Wait() {
}

void daEnFlipBlock_c::endState_Wait() {
}

void daEnFlipBlock_c::executeState_Wait() {
	int result = blockResult();

	if (result == 0)
		return;

	if (result == 1) {
		doStateChange(&daEnBlockMain_c::StateID_UpMove);
		anotherFlag = 2;
		isGroundPound = false;
	} else {
		doStateChange(&daEnBlockMain_c::StateID_DownMove);
		anotherFlag = 1;
		isGroundPound = true;
	}
}


void daEnFlipBlock_c::beginState_Flipping() {
	flipsRemaining = 7;
	physics.removeFromList();
}
void daEnFlipBlock_c::executeState_Flipping() {
	if (isGroundPound)
		rot.x += 0x800;
	else
		rot.x -= 0x800;

	if (rot.x == 0) {
		flipsRemaining--;
		if (flipsRemaining <= 0) {
			if (!playerOverlaps())
				doStateChange(&StateID_Wait);
		}
	}
}
void daEnFlipBlock_c::endState_Flipping() {
	physics.setup(this, &physicsInfo, 3, currentLayerID);
	physics.addToList();
}



bool daEnFlipBlock_c::playerOverlaps() {
	dStageActor_c *player = 0;

	Vec myBL = {pos.x - 8.0f, pos.y - 8.0f, 0.0f};
	Vec myTR = {pos.x + 8.0f, pos.y + 8.0f, 0.0f};

	while ((player = (dStageActor_c*)fBase_c::search(PLAYER, player)) != 0) {
		float centerX = player->pos.x + player->aPhysics.info.xDistToCenter;
		float centerY = player->pos.y + player->aPhysics.info.yDistToCenter;

		float left = centerX - player->aPhysics.info.xDistToEdge;
		float right = centerX + player->aPhysics.info.xDistToEdge;

		float top = centerY + player->aPhysics.info.yDistToEdge;
		float bottom = centerY - player->aPhysics.info.yDistToEdge;

		Vec playerBL = {left, bottom + 0.1f, 0.0f};
		Vec playerTR = {right, top - 0.1f, 0.0f};

		if (RectanglesOverlap(&playerBL, &playerTR, &myBL, &myTR))
			return true;
	}

	return false;
}


//
// processed\../src/magicplatform.cpp
//

#include <game.h>
#include <dCourse.h>

class daEnMagicPlatform_c : public dEn_c {
	public:
		static daEnMagicPlatform_c *build();

		int onCreate();
		int onExecute();
		int onDelete();

		enum CollisionType {
			Solid = 0,
			SolidOnTop = 1,
			None = 2,
			ThinLineRight = 3,
			ThinLineLeft = 4,
			ThinLineTop = 5,
			ThinLineBottom = 6,
			NoneWithZ500 = 7
		};

		// Settings
		CollisionType collisionType;
		u8 rectID, moveSpeed, moveDirection, moveLength;

		u8 moveDelay, currentMoveDelay;

		bool doesMoveInfinitely;

		float moveMin, moveMax, moveDelta, moveBaseDelta;
		float *moveTarget;

		bool isMoving;
		void setupMovement();
		void handleMovement();

		Physics physics;
		StandOnTopCollider sotCollider;

		TileRenderer *renderers;
		int rendererCount;

		void findSourceArea();
		void createTiles();
		void deleteTiles();
		void updateTilePositions();

		void checkVisibility();
		void setVisible(bool shown);

		bool isVisible;

		int srcX, srcY;
		int width, height;
};

/*****************************************************************************/
// Glue Code
daEnMagicPlatform_c *daEnMagicPlatform_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daEnMagicPlatform_c));
	daEnMagicPlatform_c *c = new(buffer) daEnMagicPlatform_c;
	return c;
}

extern "C" void HurtMarioBecauseOfBeingSquashed(void *mario, dStageActor_c *squasher, int type);

static void PhysCB1(daEnMagicPlatform_c *one, dStageActor_c *two) {
	if (two->stageActorType != 1)
		return;

	// if left/right
	if (one->moveDirection <= 1)
		return;

	if (one->pos_delta.y > 0.0f)
		HurtMarioBecauseOfBeingSquashed(two, one, 1);
	else
		HurtMarioBecauseOfBeingSquashed(two, one, 9);
}

static void PhysCB2(daEnMagicPlatform_c *one, dStageActor_c *two) {
	if (two->stageActorType != 1)
		return;

	// if left/right
	if (one->moveDirection <= 1)
		return;

	if (one->pos_delta.y < 0.0f)
		HurtMarioBecauseOfBeingSquashed(two, one, 2);
	else
		HurtMarioBecauseOfBeingSquashed(two, one, 10);
}

static void PhysCB3(daEnMagicPlatform_c *one, dStageActor_c *two, bool unkMaybeNotBool) {
	if (two->stageActorType != 1)
		return;

	// if up/down
	if (one->moveDirection > 1)
		return;

	if (unkMaybeNotBool) {
		if (one->pos_delta.x > 0.0f)
			HurtMarioBecauseOfBeingSquashed(two, one, 6);
		else
			HurtMarioBecauseOfBeingSquashed(two, one, 12);
	} else {
		if (one->pos_delta.x < 0.0f)
			HurtMarioBecauseOfBeingSquashed(two, one, 5);
		else
			HurtMarioBecauseOfBeingSquashed(two, one, 11);
	}
}

static bool PhysCB4(daEnMagicPlatform_c *one, dStageActor_c *two) {
	return (one->pos_delta.y > 0.0f);
}

static bool PhysCB5(daEnMagicPlatform_c *one, dStageActor_c *two) {
	return (one->pos_delta.y < 0.0f);
}

static bool PhysCB6(daEnMagicPlatform_c *one, dStageActor_c *two, bool unkMaybeNotBool) {
	if (unkMaybeNotBool) {
		if (one->pos_delta.x > 0.0f)
			return true;
	} else {
		if (one->pos_delta.x < 0.0f)
			return true;
	}
	return false;
}

int daEnMagicPlatform_c::onCreate() {
	rectID = settings & 0xFF;

	moveSpeed = (settings & 0xF00) >> 8;
	moveDirection = (settings & 0x3000) >> 12;
	moveLength = ((settings & 0xF0000) >> 16) + 1;

	moveDelay = ((settings & 0xF00000) >> 20) * 6;

	collisionType = (CollisionType)((settings & 0xF000000) >> 24);

	doesMoveInfinitely = (settings & 0x10000000);

	if (settings & 0xE0000000) {
		int putItBehind = settings >> 29;
		pos.z = -3600.0f - (putItBehind * 16);
	}
	if (collisionType == NoneWithZ500)
		pos.z = 500.0f;

	setupMovement();

	findSourceArea();
	createTiles();

	float fWidth = width * 16.0f;
	float fHeight = height * 16.0f;

	switch (collisionType) {
		case Solid:
			physics.setup(this,
					0.0f, 0.0f, fWidth, -fHeight,
					(void*)&PhysCB1, (void*)&PhysCB2, (void*)&PhysCB3, 1, 0, 0);

			physics.callback1 = (void*)&PhysCB4;
			physics.callback2 = (void*)&PhysCB5;
			physics.callback3 = (void*)&PhysCB6;

			physics.addToList();
			break;
		case SolidOnTop:
			sotCollider.init(this,
					/*xOffset=*/0.0f, /*yOffset=*/0.0f,
					/*topYOffset=*/0,
					/*rightSize=*/fWidth, /*leftSize=*/0,
					/*rotation=*/0, /*_45=*/1
					);

			// What is this for. I dunno
			sotCollider._47 = 0xA;
			sotCollider.flags = 0x80180 | 0xC00;

			sotCollider.addToList();

			break;
		case ThinLineLeft: case ThinLineRight:
		case ThinLineTop: case ThinLineBottom:
			physics.setup(this,
				fWidth * (collisionType == ThinLineRight ? 0.875f : 0.0f),
				fHeight * (collisionType == ThinLineBottom ? -0.75f : 0.0f),
				fWidth * (collisionType == ThinLineLeft ? 0.125f : 1.0f),
				fHeight * (collisionType == ThinLineTop ? -0.25f : -1.0f),
				(void*)&PhysCB1, (void*)&PhysCB2, (void*)&PhysCB3, 1, 0, 0);

			physics.callback1 = (void*)&PhysCB4;
			physics.callback2 = (void*)&PhysCB5;
			physics.callback3 = (void*)&PhysCB6;

			physics.addToList();
			break;
	}

	return 1;
}

int daEnMagicPlatform_c::onDelete() {
	deleteTiles();

	switch (collisionType) {
		case ThinLineLeft: case ThinLineRight:
		case ThinLineTop: case ThinLineBottom:
		case Solid: physics.removeFromList(); break;
	}

	return 1;
}

int daEnMagicPlatform_c::onExecute() {
	handleMovement();

	checkVisibility();

	updateTilePositions();

	switch (collisionType) {
		case ThinLineLeft: case ThinLineRight:
		case ThinLineTop: case ThinLineBottom:
		case Solid: physics.update(); break;
		case SolidOnTop: sotCollider.update(); break;
	}

	return 1;
}

/*****************************************************************************/
// Movement
void daEnMagicPlatform_c::setupMovement() {
	float fMoveLength = 16.0f * moveLength;
	float fMoveSpeed = 0.2f * moveSpeed;

	switch (moveDirection) {
		case 0: // RIGHT
			moveTarget = &pos.x;
			moveMin = pos.x;
			moveMax = pos.x + fMoveLength;
			moveBaseDelta = fMoveSpeed;
			break;
		case 1: // LEFT
			moveTarget = &pos.x;
			moveMin = pos.x - fMoveLength;
			moveMax = pos.x;
			moveBaseDelta = -fMoveSpeed;
			break;
		case 2: // UP
			moveTarget = &pos.y;
			moveMin = pos.y;
			moveMax = pos.y + fMoveLength;
			moveBaseDelta = fMoveSpeed;
			break;
		case 3: // DOWN
			moveTarget = &pos.y;
			moveMin = pos.y - fMoveLength;
			moveMax = pos.y;
			moveBaseDelta = -fMoveSpeed;
			break;
	}

	if (spriteFlagNum == 0) {
		isMoving = (moveSpeed > 0);
		moveDelta = moveBaseDelta;
	} else {
		isMoving = false;
	}

	currentMoveDelay = 0;
}

void daEnMagicPlatform_c::handleMovement() {
	if (spriteFlagNum > 0) {
		// Do event checks
		bool flagOn = ((dFlagMgr_c::instance->flags & spriteFlagMask) != 0);

		if (isMoving) {
			if (!flagOn) {
				// Flag was turned off while moving, so go back
				moveDelta = -moveBaseDelta;
			} else {
				moveDelta = moveBaseDelta;
			}
		} else {
			if (flagOn) {
				// Flag was turned on, so start moving
				moveDelta = moveBaseDelta;
				isMoving = true;
			}
		}
	}

	if (!isMoving)
		return;

	if (currentMoveDelay > 0) {
		currentMoveDelay--;
		return;
	}

	// Do it
	bool goesForward = (moveDelta > 0.0f);
	bool reachedEnd = false;

	*moveTarget += moveDelta;

	// if we're set to move infinitely, never stop
	if (doesMoveInfinitely)
		return;

	if (goesForward) {
		if (*moveTarget >= moveMax) {
			*moveTarget = moveMax;
			reachedEnd = true;
		}
	} else {
		if (*moveTarget <= moveMin) {
			*moveTarget = moveMin;
			reachedEnd = true;
		}
	}

	if (reachedEnd) {
		if (spriteFlagNum > 0) {
			// If event, just do nothing.. depending on what side we are on
			if ((moveDelta > 0.0f && moveBaseDelta > 0.0f) || (moveDelta < 0.0f && moveBaseDelta < 0.0f)) {
				// We reached the end, so keep isMoving on for when we need to reverse
			} else {
				// We're back at the start, so turn it off
				isMoving = false;
			}
		} else {
			// Otherwise, reverse
			moveDelta = -moveDelta;
			currentMoveDelay = moveDelay;
		}
	}
}

/*****************************************************************************/
// Tile Renderers

void daEnMagicPlatform_c::findSourceArea() {
	mRect rect;
	dCourseFull_c::instance->get(GetAreaNum())->getRectByID(rectID, &rect);

	// Round the positions down/up to get the rectangle
	int left = rect.x;
	int right = left + rect.width;
	int top = -rect.y;
	int bottom = top + rect.height;

	left &= 0xFFF0;
	right = (right + 15) & 0xFFF0;

	top &= 0xFFF0;
	bottom = (bottom + 15) & 0xFFF0;

	// Calculate the actual stuff
	srcX = left >> 4;
	srcY = top >> 4;
	width = (right - left) >> 4;
	height = (bottom - top) >> 4;

	//OSReport("Area: %f, %f ; %f x %f\n", rect.x, rect.y, rect.width, rect.height);
	//OSReport("Source: %d, %d ; Size: %d x %d\n", srcX, srcY, width, height);
}


void daEnMagicPlatform_c::createTiles() {
	rendererCount = width * height;
	renderers = new TileRenderer[rendererCount];

	// copy all the fuckers over
	int baseWorldX = srcX << 4, worldY = srcY << 4, rendererID = 0;

	for (int y = 0; y < height; y++) {
		int worldX = baseWorldX;

		for (int x = 0; x < width; x++) {
			u16 *pExistingTile = dBgGm_c::instance->getPointerToTile(worldX, worldY, 0);

			if (*pExistingTile > 0) {
				TileRenderer *r = &renderers[rendererID];
				r->tileNumber = *pExistingTile;
				r->z = pos.z;
			}

			worldX += 16;
			rendererID++;
		}

		worldY += 16;
	}

}

void daEnMagicPlatform_c::deleteTiles() {
	if (renderers != 0) {
		setVisible(false);

		delete[] renderers;
		renderers = 0;
	}
}

void daEnMagicPlatform_c::updateTilePositions() {
	float baseX = pos.x;

	float y = -pos.y;

	int rendererID = 0;

	for (int yIdx = 0; yIdx < height; yIdx++) {
		float x = baseX;

		for (int xIdx = 0; xIdx < width; xIdx++) {
			TileRenderer *r = &renderers[rendererID];
			r->x = x;
			r->y = y;

			x += 16.0f;
			rendererID++;
		}

		y += 16.0f;
	}
}



void daEnMagicPlatform_c::checkVisibility() {
	float effectiveLeft = pos.x, effectiveRight = pos.x + (width * 16.0f);
	float effectiveBottom = pos.y - (height * 16.0f), effectiveTop = pos.y;

	ClassWithCameraInfo *cwci = ClassWithCameraInfo::instance;

	float screenRight = cwci->screenLeft + cwci->screenWidth;
	float screenBottom = cwci->screenTop - cwci->screenHeight;

	bool isOut = (effectiveLeft > screenRight) ||
		(effectiveRight < cwci->screenLeft) ||
		(effectiveTop < screenBottom) ||
		(effectiveBottom > cwci->screenTop);

	setVisible(!isOut);
}

void daEnMagicPlatform_c::setVisible(bool shown) {
	if (isVisible == shown)
		return;
	isVisible = shown;

	TileRenderer::List *list = dBgGm_c::instance->getTileRendererList(0);

	for (int i = 0; i < rendererCount; i++) {
		if (renderers[i].tileNumber > 0) {
			if (shown) {
				list->add(&renderers[i]);
			} else {
				list->remove(&renderers[i]);
			}
		}
	}
}


//
// processed\../src/cutScene.cpp
//

#include "cutScene.h"

dScCutScene_c *dScCutScene_c::instance = 0;

dScCutScene_c *dScCutScene_c::build() {
	// return new dScCutScene_c;
	void *buffer = AllocFromGameHeap1(sizeof(dScCutScene_c));
	dScCutScene_c *c = new(buffer) dScCutScene_c;

	instance = c;
	return c;
}

dScCutScene_c::dScCutScene_c() {
	data = 0;
	layout = 0;
	sceneLoaders = 0;

	Actors aYesNoWindow = translateActorID(YES_NO_WINDOW);
	yesNoWindow = (dYesNoWindow_c*)CreateParentedObject(aYesNoWindow, this, 0, 0);
	Actors aSelectCursor = translateActorID(SELECT_CURSOR);
	CreateParentedObject(aSelectCursor, this, 0, 0);
}

dScCutScene_c::~dScCutScene_c() {
	if (layout)
		delete layout;

	if (sceneLoaders)
		delete[] sceneLoaders;
}


static const char *CutsceneNames[] = {
	"/CS/Opening.cs",
	"/CS/Kamek.cs",
	"/CS/Ending.cs"
};

static const char *WideCutsceneNames[] = {
	"/CS/WideOpening.cs",
	"/CS/Kamek.cs",
	"/CS/Ending.cs"
};


int dScCutScene_c::onCreate() {
	*CurrentDrawFunc = CutSceneDrawFunc;

	currentScene = -1;

	int csNumber = settings >> 28;
	const char *csName = IsWideScreen() ?
		WideCutsceneNames[csNumber] : CutsceneNames[csNumber];

	if (settingsLoader.load(csName)) {
		// only deal with this once!
		if (data) return 1;

		data = (dMovieData_s*)settingsLoader.buffer;

		// fix up the settings
		for (int i = 0; i < data->sceneCount; i++) {
			data->scenes[i] =
				(dMovieScene_s*)((u32)data + (u32)data->scenes[i]);

			data->scenes[i]->sceneName =
				(char*)((u32)data + (u32)data->scenes[i]->sceneName);
		}

		sceneLoaders = new dDvdLoader_c[data->sceneCount];

		nextScene = 0;

		return 1;
	}

	return 0;
}

int dScCutScene_c::onDelete() {
	if (layout)
		return layout->free();

	for (int i = 0; i < data->sceneCount; i++) {
		sceneLoaders[i].unload();
	}
	settingsLoader.unload();

	return true;
}

int dScCutScene_c::onExecute() {
	int nowPressed = Remocon_GetPressed(GetActiveRemocon());

	switch (yesNoStage) {
		case 1:
			// Opening
			if (!yesNoWindow->animationActive)
				yesNoStage = 2;
			return true;
		case 2:
			// Opened
			if (nowPressed & WPAD_LEFT)
				yesNoWindow->current = 1;
			else if (nowPressed & WPAD_RIGHT)
				yesNoWindow->current = 0;
			else if (Wiimote_TestButtons(GetActiveWiimote(), WPAD_A | WPAD_TWO)) {
				yesNoWindow->close = true;

				if (yesNoWindow->current != 1)
					yesNoWindow->hasBG = true;
				yesNoStage = 3;

			} else {
				// Cancel using B or 1
				if (CheckIfMenuShouldBeCancelledForSpecifiedWiimote(0)) {
					yesNoWindow->cancelled = true;
					yesNoWindow->current = 1;
					yesNoStage = 3;
				}
			}
			return true;
		case 3:
			// Closing
			if (!yesNoWindow->visible) {
				if (yesNoWindow->current == 1) {
					yesNoStage = 0;
					for (int i = 0; i < 40; i++)
						if (handles[i].Exists()) {
							handles[i].Pause(false, 3);
						}
				} else {
					yesNoStage = 4;
					goToNextScene();
				}
			}
			return true;
		case 4:
			// Waiting for exit transition
			return true;
	}

	if (yesNoWindow->layoutLoaded && (nowPressed & WPAD_PLUS)) {
		yesNoStage = 1;
		yesNoWindow->type = 3;
		yesNoWindow->visible = true;

		for (int i = 0; i < 40; i++)
			if (handles[i].Exists()) {
				handles[i].Pause(true, 3);
			}
		return true;
	}

	// deal with loading first

	// what do we want to load?
	if (currentScene >= 0 || nextScene >= 0) {
		int whatToLoad = (nextScene >= 0) ? nextScene : (currentScene + 1);

		sceneLoaders[whatToLoad].load(data->scenes[whatToLoad]->sceneName);
	}


	// now, do all other processing

	if (currentScene >= 0) {
		if (!layout->isAnyAnimOn()) {
			// we're at the end
			// what now?

			if ((currentScene + 1) == data->sceneCount) {
				goToNextScene();
			} else {
				nextScene = currentScene + 1;
				OSReport("switching to scene %d\n", nextScene);
			}

			sceneLoaders[currentScene].unload();
			currentScene = -1;
			layout->loader.buffer = 0;
			layout->free();
			delete layout;
			layout = 0;
			return true;
		}

		frameOffset++;

		// check if we gotta do anything
		dMovieScene_s *scene = data->scenes[currentScene];
		int cmdsParsed = 0;
		int cmdOffset = 0;
		while (cmdsParsed < scene->commandCount) {
			u32 cmdType = scene->command[cmdOffset++];
			cmdsParsed++;

			switch (cmdType) {
				case 1:
					{
						u32 delay = scene->command[cmdOffset++];
						u32 soundID = scene->command[cmdOffset++];
						if (delay != frameOffset)
							continue;

						nw4r::snd::SoundHandle *handle = 0;
						for (int i = 20; i < 40; i++) {
							if (!handles[i].Exists()) {
								handle = &handles[i];
								break;
							}
						}
						if (handle)
							PlaySoundWithFunctionB4(SoundRelatedClass, handle, soundID, 1);
					} break;
				case 2:
					{
						u32 handleID = scene->command[cmdOffset++];
						u32 delay = scene->command[cmdOffset++];
						u32 soundID = scene->command[cmdOffset++];
						if (delay != frameOffset)
							continue;

						PlaySoundWithFunctionB4(SoundRelatedClass, &handles[handleID], soundID, 1);
					} break;
				case 3:
					{
						u32 handleID = scene->command[cmdOffset++];
						u32 delay = scene->command[cmdOffset++];
						u32 frameCount = scene->command[cmdOffset++];
						if (delay != frameOffset)
							continue;

						if (handles[handleID].Exists()) {
							handles[handleID].Stop(frameCount);
							handles[handleID].DetachSound();
						}
					} break;
			}
		}

		if (nowPressed & WPAD_ONE) {
			OSReport("Currently: Scene %d; Frame %d\n", currentScene, frameOffset);
		}

		layout->execAnimations();
		layout->update();
	}
	
	if (nextScene >= 0) {
		// is this scene loaded yet?
		if (sceneLoaders[nextScene].buffer) {
			currentScene = nextScene;

			OSReport("Loading scene %d\n", currentScene);

			layout = new m2d::EmbedLayout_c;
			layout->loader.buffer = sceneLoaders[nextScene].buffer;
			layout->loader.attachArc(layout->loader.buffer, "arc");
			layout->resAccPtr = &layout->loader;

			bool result = layout->build("cutscene.brlyt");
			OSReport("Result: %d\n", result);
			layout->loadAnimations((const char *[1]){"cutscene.brlan"}, 1);
			layout->loadGroups((const char *[1]){"cutscene"}, (int[1]){0}, 1);
			layout->disableAllAnimations();
			layout->enableNonLoopAnim(0);

			u8 widescreenFlag = data->scenes[nextScene]->widescreenFlag;
			if (widescreenFlag == 1 || widescreenFlag == 3) {
				// Native on 16:9, letterboxed on 4:3
				if (IsWideScreen()) {
					if (widescreenFlag == 3) {
						layout->layout.rootPane->trans.x = -100.0f;
						layout->layout.rootPane->scale.x = 0.75f;
					}
				} else {
					layout->clippingEnabled = true;
					layout->clipX = 0;
					layout->clipY = 52;
					layout->clipWidth = 640;
					layout->clipHeight = 352;
					layout->layout.rootPane->scale.x = 0.7711f;
					layout->layout.rootPane->scale.y = 0.7711f;
				}
			} else if (widescreenFlag == 0) {
				// Native on 4:3, black bars on 16:9
				if (IsWideScreen()) {
					layout->clippingEnabled = true;
					layout->clipX = 66;
					layout->clipY = 0;
					layout->clipWidth = 508;
					layout->clipHeight = 456;
					layout->layout.rootPane->scale.x = 0.794f;
				}
			}

			layout->execAnimations();
			layout->update();

			OSReport("Loaded scene %d\n", currentScene);

			nextScene = -1;
			frameOffset = 0;
		}
	}

	return true;
}

int dScCutScene_c::onDraw() {
	if (currentScene >= 0)
		layout->scheduleForDrawing();

	return true;
}

void CutSceneDrawFunc() {
	Reset3DState();
	SetupLYTDrawing();
	DrawAllLayoutsBeforeX(0x81);
	RenderEffects(0, 3);
	RenderEffects(0, 2);
	GXDrawDone();
	RemoveAllFromScnRoot();
	Reset3DState();
	SetCurrentCameraID(1);
	DoSpecialDrawing1();
	SetCurrentCameraID(0);
	for (int i = 0; i < 4; i++)
		RenderEffects(0, 0xB+i);
	for (int i = 0; i < 4; i++)
		RenderEffects(0, 7+i);
	GXDrawDone();
	// Leaving out some stuff here
	DrawAllLayoutsAfterX(0x80);
	ClearLayoutDrawList();
	SetCurrentCameraID(0);
}


void dScCutScene_c::goToNextScene() {
	// we're TOTALLY done!
	OSReport("playback complete\n");
	int nsmbwMovieType = settings & 3;
	int newerMovieType = settings >> 28;

	for (int i = 0; i < 40; i++)
		if (handles[i].Exists())
			handles[i].Stop(5);

	switch (newerMovieType) {
		case 0:
			// OPENING
			switch (nsmbwMovieType) {
				case 0:
					SaveGame(0, false);
					DoSceneChange(WORLD_MAP, 0x80000000, 0);
					break;
				case 1:
					StartTitleScreenStage(false, 0);
					break;
			}
			break;

		case 1:
			// KAMEK (W7 => W8)
			ActivateWipe(WIPE_MARIO);
			DoSceneChange(WORLD_MAP, 0x40000000, 0);
			break;
	}
}


//
// processed\../src/sceneGlue.cpp
//

#include <game.h>
#include <stage.h>

extern char CurrentLevel, CurrentWorld;
extern u8 MaybeFinishingLevel[2];
extern "C" void ExitStageReal(int scene, int sceneParams, int powerupStoreType, int wipe);


extern "C" void ExitStageWrapper(int scene, int sceneParams, int powerupStoreType, int wipe) {
	// TO RE-ENABLE CUTSCENES, UNCOMMENT THIS
	// if (scene == WORLD_MAP && powerupStoreType == BEAT_LEVEL) {
	// 	if (CurrentWorld == 6 && CurrentLevel == STAGE_DOOMSHIP) {
	// 		if (MaybeFinishingLevel[0] == 6 && MaybeFinishingLevel[1] == STAGE_DOOMSHIP) {
	// 			// We're done with 7-38
	// 			ExitStage(MOVIE, 0x10000000, powerupStoreType, wipe);
	// 			return;
	// 		}
	// 	}
	// }

	ExitStageReal(scene, sceneParams, powerupStoreType, wipe);
}


extern "C" void EnterOpeningLevel() {
	DontShowPreGame = true;
	RESTART_CRSIN_LevelStartStruct.purpose = 0;
	RESTART_CRSIN_LevelStartStruct.world1 = 1;
	RESTART_CRSIN_LevelStartStruct.world2 = 1;
	RESTART_CRSIN_LevelStartStruct.level1 = 40;
	RESTART_CRSIN_LevelStartStruct.level2 = 40;
	RESTART_CRSIN_LevelStartStruct.areaMaybe = 0;
	RESTART_CRSIN_LevelStartStruct.entrance = 0xFF;
	RESTART_CRSIN_LevelStartStruct.unk4 = 0; // load replay
	DoSceneChange(RESTART_CRSIN, 0, 0);
}


//
// processed\../src/bonepiece.cpp
//

#include <game.h>

class daBonePiece_c : public dStageActor_c {
	public:
		static daBonePiece_c *build();

		int onCreate();
		int onExecute();
		int onDraw();
		int onDelete();

		StandOnTopCollider collider;

		nw4r::g3d::ResFile resFile;
		mHeapAllocator_c allocator;
		m3d::mdl_c model;
};

/*****************************************************************************/
// Glue Code
daBonePiece_c *daBonePiece_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daBonePiece_c));
	daBonePiece_c *c = new(buffer) daBonePiece_c;
	return c;
}

int daBonePiece_c::onCreate() {
	// load the model
	allocator.link(-1, GameHeaps[0], 0, 0x20);

	resFile.data = getResource("lift_torokko", "g3d/t00.brres");

	static char thing[] = "lift_torokko?";
	thing[0xC] = 'A' + (settings & 3);

	nw4r::g3d::ResMdl resmdl = resFile.GetResMdl(thing);
	model.setup(resmdl, &allocator, 0, 1, 0);
	SetupTextures_MapObj(&model, 0);

	allocator.unlink();

	// if rotation is off, do nothing else
	if ((settings >> 28) & 1) {
		// OK, figure out the rotation
		u8 sourceRotation = (settings >> 24) & 0xF;

		// 0 is up. -0x4000 is right, 0x4000 is left ...
		s16 rotation;

		// We'll flip it later.
		// Thus: 0..7 rotates left (in increments of 0x800),
		// 8..15 rotates right (in increments of 0x800 too).
		// To specify facing up, well.. just use 0.

		if (sourceRotation < 8)
			rotation = (sourceRotation * 0x800) - 0x4000;
		else
			rotation = (sourceRotation * 0x800) - 0x3800;

		rotation = -rotation;

		rot.z = rotation;
	}

	if ((settings >> 20) & 1) {
		rot.y = 0x8000;
	}

	collider.init(this,
			/*xOffset=*/0.0f, /*yOffset=*/0.0f,
			/*topYOffset=*/0,
			/*rightSize=*/16.0f, /*leftSize=*/-16.0f,
			/*rotation=*/rot.z, /*_45=*/1
			);

	collider._47 = 0xA;
	collider.flags = 0x80180 | 0xC00;

	collider.addToList();

	return true;
}

int daBonePiece_c::onDelete() {
	return true;
}

int daBonePiece_c::onExecute() {
	matrix.translation(pos.x, pos.y - 8.0f, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	model.setDrawMatrix(matrix);
	model.setScale(&scale);
	model.calcWorld(false);

	collider.update();

	return true;
}

int daBonePiece_c::onDraw() {
	model.scheduleForDrawing();

	return true;
}




//
// processed\../src/music.cpp
//

#include <game.h>
#include <sfx.h>
#include "music.h"

struct HijackedStream {
	//const char *original;
	//const char *originalFast;
	u32 stringOffset;
	u32 stringOffsetFast;
	u32 infoOffset;
	u8 originalID;
	int streamID;
};

struct Hijacker {
	HijackedStream stream[2];
	u8 currentStream;
	u8 currentCustomTheme;
};



const char* SongNameList [] = {
	"AIRSHIP",
	"BOSS_TOWER",
	"MENU",
	"UNDERWATER",
	"ATHLETIC",
	"CASTLE",
	"MAIN",
	"MOUNTAIN",
	"TOWER",
	"UNDERGROUND",
	"DESERT",
	"FIRE",
	"FOREST",
	"FREEZEFLAME",
	"JAPAN",
	"PUMPKIN",
	"SEWER",
	"SPACE",
	"BOWSER",
	"BONUS",	
	"AMBUSH",	
	"BRIDGE_DRUMS",	
	"SNOW2",	
	"MINIMEGA",	
	"CLIFFS",
	"AUTUMN",
	"CRYSTALCAVES",
	"GHOST_HOUSE",
	"GRAVEYARD",
	"JUNGLE",
	"TROPICAL",
	"SKY_CITY",
	"SNOW",
	"STAR_HAVEN",
	"SINGALONG",
	"FACTORY",
	"TANK",
	"TRAIN",
	"YOSHIHOUSE",
	"FACTORYB",
	"CAVERN",
	"SAND",
	"SHYGUY",
	"MINIGAME",
	"BONUS_AREA",
	"CHALLENGE",
	"BOWSER_CASTLE",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"BOSS_CASTLE",
	"BOSS_AIRSHIP",
	NULL	
};



// Offsets are from the start of the INFO block, not the start of the brsar.
// INFO begins at 0x212C0, so that has to be subtracted from absolute offsets
// within the brsar.

#define _I(offs) ((offs)-0x212C0)

Hijacker Hijackers[2] = {
	{
		{
			{/*"athletic_lr.n.32.brstm", "athletic_fast_lr.n.32.brstm",*/ _I(0x4A8F8), _I(0x4A938), _I(0x476C4), 4, STRM_BGM_ATHLETIC},
			{/*"BGM_SIRO.32.brstm", "BGM_SIRO_fast.32.brstm",*/ _I(0x4B2E8), _I(0x4B320), _I(0x48164), 10, STRM_BGM_SHIRO}
		},
		0, 0
	},

	{
		{
			{/*"STRM_BGM_CHIJOU.brstm", "STRM_BGM_CHIJOU_FAST.brstm",*/ _I(0x4A83C), _I(0x4A8B4), 0, 1, STRM_BGM_CHIJOU},
			{/*"STRM_BGM_CHIKA.brstm", "STRM_BGM_CHIKA_FAST.brstm",*/ _I(0x4A878), _I(0x4A780), 0, 2, STRM_BGM_CHIKA},
		},
		0, 0
	}
};

extern void *SoundRelatedClass;
inline char *BrsarInfoOffset(u32 offset) {
	return (char*)(*(u32*)(((u32)SoundRelatedClass) + 0x5CC)) + offset;
}

void FixFilesize(u32 streamNameOffset);

u8 hijackMusicWithSongName(const char *songName, int themeID, bool hasFast, int channelCount, int trackCount, int *wantRealStreamID) {
	Hijacker *hj = &Hijackers[channelCount==4?1:0];

	// do we already have this theme in this slot?
	// if so, don't switch streams
	// if we do, NSMBW will think it's a different song, and restart it ...
	// but if it's just an area transition where both areas are using the same
	// song, we don't want that
	if ((themeID >= 0) && hj->currentCustomTheme == themeID)
		return hj->stream[hj->currentStream].originalID;

	// which one do we use this time...?
	int toUse = (hj->currentStream + 1) & 1;

	hj->currentStream = toUse;
	hj->currentCustomTheme = themeID;

	// write the stream's info
	HijackedStream *stream = &hj->stream[hj->currentStream];

	if (stream->infoOffset) {
		u16 *thing = (u16*)(BrsarInfoOffset(stream->infoOffset) + 4);
		OSReport("Modifying stream info, at offset %x which is at pointer %x\n", stream->infoOffset, thing);
		OSReport("It currently has: channel count %d, track bitfield 0x%x\n", thing[0], thing[1]);
		thing[0] = channelCount;
		thing[1] = (1 << trackCount) - 1;
		OSReport("It has been set to: channel count %d, track bitfield 0x%x\n", thing[0], thing[1]);
	}

	sprintf(BrsarInfoOffset(stream->stringOffset), "new/%s.er", songName);
	sprintf(BrsarInfoOffset(stream->stringOffsetFast), hasFast?"new/%s_F.er":"new/%s.er", songName);

	// update filesizes
	FixFilesize(stream->stringOffset);
	FixFilesize(stream->stringOffsetFast);

	// done!
	if (wantRealStreamID)
		*wantRealStreamID = stream->streamID;

	return stream->originalID;
}


//oh for fuck's sake
#include "fileload.h"

void FixFilesize(u32 streamNameOffset) {
	char *streamName = BrsarInfoOffset(streamNameOffset);

	char nameWithSound[80];
	snprintf(nameWithSound, 79, "/Sound/%s", streamName);

	s32 entryNum;
	DVDHandle info;
	
	if ((entryNum = DVDConvertPathToEntrynum(nameWithSound)) >= 0) {
		if (DVDFastOpen(entryNum, &info)) {
			u32 *lengthPtr = (u32*)(streamName - 0x1C);
			*lengthPtr = info.length;
		}
	} else
		OSReport("What, I couldn't find \"%s\" :(\n", nameWithSound);
}



extern "C" u8 after_course_getMusicForZone(u8 realThemeID) {
	if (realThemeID < 100)
		return realThemeID;

	bool usesDrums = (realThemeID >= 200);
	return hijackMusicWithSongName(SongNameList[realThemeID-100], realThemeID, true, usesDrums?4:2, usesDrums?2:1, 0);
}
//
// processed\../src/newer.cpp
//

#include <game.h>
#include <newer.h>
#include "levelinfo.h"

int lastLevelIDs[] = {
	-1, /*no world*/
	27, 27, 27, 27, 27, 27, 27, 25,
	10,
	24, 24, 21, 24, 3
};


/*
extern "C" void FuckUpYoshi(void *_this) {
	dEn_c *koopa = (dEn_c*)fBase_c::search(EN_NOKONOKO, 0);
	static int thing = 0;
	thing++;
	nw4r::db::Exception_Printf_("Fruit eaten: %d\n", thing);
	if (thing == 5) {
		nw4r::db::Exception_Printf_("5th fruit eaten\n");
		nw4r::db::Exception_Printf_("Let's try fucking up Yoshi!\n");
		daPlBase_c *yoshi = (daPlBase_c*)fBase_c::search(YOSHI, 0);
		nw4r::db::Exception_Printf_("Fruit: %p ; Koopa: %p ; Yoshi: %p\n", _this, koopa, yoshi);
		koopa->_vf220(yoshi);
		nw4r::db::Exception_Printf_("Yoshi fucked up. Yay.\n");
		thing = 0;
	}
}

extern "C" void FuckUpYoshi2() {
	dEn_c *koopa = (dEn_c*)fBase_c::search(EN_NOKONOKO, 0);
	nw4r::db::Exception_Printf_("Let's try fucking up Yoshi!\n");
	daPlBase_c *yoshi = (daPlBase_c*)fBase_c::search(YOSHI, 0);
	koopa->_vf220(yoshi);
	nw4r::db::Exception_Printf_("Yoshi fucked up. Yay.\n");
}

extern "C" void StartAnimOrig(dPlayerModelBase_c *_this, int id, float updateRate, float unk, float frame);
extern "C" void YoshiStartAnimWrapper(dPlayerModelBase_c *_this, int id, float updateRate, float unk, float frame) {
	nw4r::db::Exception_Printf_("[%d] anim %d (%f, %f, %f)\n", GlobalTickCount, id, updateRate, unk, frame);
	StartAnimOrig(_this, id, updateRate, unk, frame);
}
extern "C" void YoshiStateOrig(daPlBase_c *_this, dStateBase_c *state, void *param);
extern "C" void YoshiStateWrapper(daPlBase_c *_this, dStateBase_c *state, void *param) {
	nw4r::db::Exception_Printf_("[%d] %s,%p\n", GlobalTickCount, state->getName(), param);
	YoshiStateOrig(_this, state, param);
}
*/


void WriteAsciiToTextBox(nw4r::lyt::TextBox *tb, const char *source) {
	int i = 0;
	wchar_t buffer[1024];
	while (i < 1023 && source[i]) {
		buffer[i] = source[i];
		i++;
	}
	buffer[i] = 0;

	tb->SetString(buffer);
}


void getNewerLevelNumberString(int world, int level, wchar_t *dest) {
	static const wchar_t *numberKinds[] = {
		// 0-19 are handled by code
		// To insert a picturefont character:
		// \x0B\x01YY\xZZZZ
		// YY is the character code, ZZZZ is ignored
		L"A", // 20, alternate
		L"\x0B\x0148\xBEEF", // 21, tower
		L"\x0B\x0148\xBEEF" L"2", // 22, tower 2
		L"\x0B\x012E\xBEEF", // 23, castle
		L"\x0B\x012F\xBEEF", // 24, fortress
		L"\x0B\x013D\xBEEF", // 25, final castle
		L"\x0B\x014D\xBEEF", // 26, train
		L"\x0B\x0132\xBEEF", // 27, airship
		L"Palace", // 28, switch palace
		L"\x0B\x0147\xBEEF", // 29, yoshi's house
		L"\x0B\x014E\xBEEF" L"1", // 30, key 1
		L"\x0B\x014E\xBEEF" L"2", // 31, key 2
		L"\x0B\x014E\xBEEF" L"3", // 32, key 3
		L"\x0B\x014E\xBEEF" L"4", // 33, key 4
		L"\x0B\x014E\xBEEF" L"5", // 34, key 5
		L"\x0B\x014E\xBEEF" L"6", // 35, key 6
		L"\x0B\x0138\xBEEF", // 36, music house
		L"\x0B\x0133\xBEEF", // 37, shop
		L"\x0B\x0139\xBEEF", // 38, challenge house
		L"\x0B\x0151\xBEEF", // 39, red switch palace
		L"\x0B\x0152\xBEEF", // 40, blue switch palace
		L"\x0B\x0153\xBEEF", // 41, yellow switch palace
		L"\x0B\x0154\xBEEF", // 42, green switch palace
		L"\x0B\x0149\xBEEF", // 43, tank
		L"\x0B\x0155\xBEEF", // 44, moai
	};

	dest[0] = (world >= 10) ? (world-10+'A') : (world+'0');
	dest[1] = '-';
	if (level >= 20) {
		wcscpy(&dest[2], numberKinds[level-20]);
	} else if (level >= 10) {
		dest[2] = '1';
		dest[3] = ('0' - 10) + level;
		dest[4] = 0;
	} else {
		dest[2] = '0' + level;
		dest[3] = 0;
	}
}

int getUnspentStarCoinCount() {
	SaveBlock *save = GetSaveFile()->GetBlock(-1);
	int coinsSpent = save->spentStarCoins;
	return getStarCoinCount() - coinsSpent;
}

int getStarCoinCount() {
	SaveBlock *save = GetSaveFile()->GetBlock(-1);
	int coinsEarned = 0;

	for (int w = 0; w < 10; w++) {
		for (int l = 0; l < 42; l++) {
			u32 conds = save->GetLevelCondition(w, l);

			if (conds & COND_COIN1) { coinsEarned++; }
			if (conds & COND_COIN2) { coinsEarned++; }
			if (conds & COND_COIN3) { coinsEarned++; }
		}
	}

	return coinsEarned;
}


struct GEIFS {
	int starCoins, exits;
};
extern "C" GEIFS *GrabExitInfoForFileSelect(GEIFS *out, SaveBlock *save) {
	out->starCoins = 0;
	out->exits = 0;

	for (int i = 0; i < dLevelInfo_c::s_info.sectionCount(); i++) {
		dLevelInfo_c::section_s *section = dLevelInfo_c::s_info.getSectionByIndex(i);

		for (int j = 0; j < section->levelCount; j++) {
			dLevelInfo_c::entry_s *l = &section->levels[j];
			if (l->flags & 2) {
				//OSReport("Checking %d-%d...\n", l->worldSlot+1, l->levelSlot+1);
				u32 cond = save->GetLevelCondition(l->worldSlot, l->levelSlot);
				if ((l->flags & 0x10) && (cond & COND_NORMAL))
					out->exits++;
				if ((l->flags & 0x20) && (cond & COND_SECRET))
					out->exits++;
				if (cond & COND_COIN1)
					out->starCoins++;
				if (cond & COND_COIN2)
					out->starCoins++;
				if (cond & COND_COIN3)
					out->starCoins++;
			}
		}
	}

	OSReport("Done, got %d coins and %d exits\n", out->starCoins, out->exits);

	return out;
}



//
// processed\../src/boss.cpp
//

#include "boss.h"



void DamagePlayer(dEn_c *actor, ActivePhysics *apThis, ActivePhysics *apOther) {

	actor->dEn_c::playerCollision(apThis, apOther);
	actor->_vf220(apOther->owner);

	// fix multiple player collisions via megazig
	actor->deathInfo.isDead = 0;
	actor->flags_4FC |= (1<<(31-7));
	if (apOther->owner->which_player == 255 ) {
		actor->counter_504[0] = 0;
		actor->counter_504[1] = 0;
		actor->counter_504[2] = 0;
		actor->counter_504[3] = 0;
	}
	else {
		actor->counter_504[apOther->owner->which_player] = 0;
	}
}


void SetupKameck(daBoss *actor, daKameckDemo *Kameck) {

	// Stop the BGM Music
	StopBGMMusic();

	// Set the necessary Flags and make Mario enter Demo Mode
	dStage32C_c::instance->freezeMarioBossFlag = 1;
	WLClass::instance->_4 = 4;
	WLClass::instance->_8 = 0;

	MakeMarioEnterDemoMode();

	// Make sure to use the correct position
	Vec pos = (Vec){actor->pos.x - 124.0, actor->pos.y + 104.0, 3564.0};
	S16Vec rot = (S16Vec){0, 0, 0};

	// Create And use Kameck
	actor->Kameck = (daKameckDemo*)actor->createChild(KAMECK_FOR_CASTLE_DEMO, (dStageActor_c*)actor, 0, &pos, &rot, 0);
	actor->Kameck->doStateChange(&daKameckDemo::StateID_DemoWait);	

}


void CleanupKameck(daBoss *actor, daKameckDemo *Kameck) {
	// Clean up the flags and Kameck
	dStage32C_c::instance->freezeMarioBossFlag = 0;
	WLClass::instance->_8 = 1;

	MakeMarioExitDemoMode();
	StartBGMMusic();

	actor->Kameck->Delete(1);
}


bool GrowBoss(daBoss *actor, daKameckDemo *Kameck, float initialScale, float endScale, float yPosModifier, int timer) {
	if (timer == 130) { actor->Kameck->doStateChange(&daKameckDemo::StateID_DemoSt); }
	if (timer == 400) { actor->Kameck->doStateChange(&daKameckDemo::StateID_DemoSt2); }

	float scaleSpeed, yPosScaling;

	if (timer == 150) { PlaySound(actor, SE_BOSS_IGGY_WANWAN_TO_L);  }
	
	if ((timer > 150) && (timer < 230)) {
		scaleSpeed = (endScale -initialScale) / 80.0;
	
		float modifier;

		modifier = initialScale + ((timer - 150) * scaleSpeed);
		
		actor->scale = (Vec){modifier, modifier, modifier};
		actor->pos.y = actor->pos.y + (yPosModifier/80.0);
	}

	if (timer == 360) { 
		Vec tempPos = (Vec){actor->pos.x - 40.0, actor->pos.y + 120.0, 3564.0};
		SpawnEffect("Wm_ob_greencoinkira", 0, &tempPos, &(S16Vec){0,0,0}, &(Vec){1.0, 1.0, 1.0});
		SpawnEffect("Wm_mr_yoshiicehit_a", 0, &tempPos, &(S16Vec){0,0,0}, &(Vec){1.0, 1.0, 1.0});
		SpawnEffect("Wm_mr_yoshiicehit_b", 0, &tempPos, &(S16Vec){0,0,0}, &(Vec){1.0, 1.0, 1.0});
		SpawnEffect("Wm_ob_redringget", 0, &tempPos, &(S16Vec){0,0,0}, &(Vec){1.0, 1.0, 1.0});
		SpawnEffect("Wm_ob_keyget01", 0, &tempPos, &(S16Vec){0,0,0}, &(Vec){1.0, 1.0, 1.0});
		SpawnEffect("Wm_ob_greencoinkira_a", 0, &tempPos, &(S16Vec){0,0,0}, &(Vec){1.0, 1.0, 1.0});
		SpawnEffect("Wm_ob_keyget01_c", 0, &tempPos, &(S16Vec){0,0,0}, &(Vec){1.0, 1.0, 1.0});
	}

	if (timer > 420) { return true; }
	return false;
}


void OutroSetup(daBoss *actor) {
	actor->removeMyActivePhysics();

	StopBGMMusic();

	WLClass::instance->_4 = 5;
	WLClass::instance->_8 = 0;
	dStage32C_c::instance->freezeMarioBossFlag = 1;

	nw4r::snd::SoundHandle handle;
	PlaySoundWithFunctionB4(SoundRelatedClass, &handle, SE_BOSS_CMN_DAMAGE_LAST, 1);
}


bool ShrinkBoss(daBoss *actor, Vec *pos, float scale, int timer) {
	// Adjust actor to equal the scale of your boss / 80.
	actor->scale.x -= scale / 80.0;
	actor->scale.y -= scale / 80.0;
	actor->scale.z -= scale / 80.0;

	// actor->pos.y += 2.0;
	
	if (timer == 30) {  
		SpawnEffect("Wm_ob_starcoinget_gl", 0, pos, &(S16Vec){0,0,0}, &(Vec){2.0, 2.0, 2.0});
		SpawnEffect("Wm_mr_vshipattack_hosi", 0, pos, &(S16Vec){0,0,0}, &(Vec){2.0, 2.0, 2.0});
		SpawnEffect("Wm_ob_keyget01_b", 0, pos, &(S16Vec){0,0,0}, &(Vec){2.0, 2.0, 2.0});
	}

	if (actor->scale.x < 0) { return true; }
	else { return false; }
}


void BossExplode(daBoss *actor, Vec *pos) {
	actor->scale.x = 0.0;
	actor->scale.y = 0.0;
	actor->scale.z = 0.0;
	
	SpawnEffect("Wm_ob_keyget02", 0, pos, &(S16Vec){0,0,0}, &(Vec){2.0, 2.0, 2.0});
	actor->dying = 1;
	actor->timer = 0;

	nw4r::snd::SoundHandle handle;
	PlaySoundWithFunctionB4(SoundRelatedClass, &handle, STRM_BGM_SHIRO_BOSS_CLEAR, 1);

	//MakeMarioEnterDemoMode();
	BossGoalForAllPlayers();
}

void BossGoalForAllPlayers() {
	for (int i = 0; i < 4; i++) {
		daPlBase_c *player = GetPlayerOrYoshi(i);
		if (player)
			player->setAnimePlayStandardType(2);
	}
}


void PlayerVictoryCries(daBoss *actor) {
	UpdateGameMgr();
	/*nw4r::snd::SoundHandle handle1, handle2, handle3, handle4;

	dAcPy_c *players[4];
	for (int i = 0; i < 4; i++)
		players[i] = (dAcPy_c *)GetSpecificPlayerActor(i);

	if (players[0] && strcmp(players[0]->states2.getCurrentState()->getName(), "dAcPy_c::StateID_Balloon"))
		PlaySoundWithFunctionB4(SoundRelatedClass, &handle1, SE_VOC_MA_CLEAR_BOSS, 1);
	if (players[1] && strcmp(players[1]->states2.getCurrentState()->getName(), "dAcPy_c::StateID_Balloon"))
		PlaySoundWithFunctionB4(SoundRelatedClass, &handle2, SE_VOC_LU_CLEAR_BOSS, 1);
	if (players[2] && strcmp(players[2]->states2.getCurrentState()->getName(), "dAcPy_c::StateID_Balloon"))
		PlaySoundWithFunctionB4(SoundRelatedClass, &handle3, SE_VOC_KO_CLEAR_BOSS, 1);
	if (players[3] && strcmp(players[3]->states2.getCurrentState()->getName(), "dAcPy_c::StateID_Balloon"))
		PlaySoundWithFunctionB4(SoundRelatedClass, &handle4, SE_VOC_KO2_CLEAR_BOSS, 1);*/
}

//
// processed\../src/fileload.cpp
//

#include "fileload.h"


void *LoadFile(FileHandle *handle, const char *name) {

	int entryNum = DVDConvertPathToEntrynum(name);

	DVDHandle dvdhandle;
	if (!DVDFastOpen(entryNum, &dvdhandle)) {
		return 0;
	}

	handle->length = dvdhandle.length;
	handle->filePtr = EGG__Heap__alloc((handle->length+0x1F) & ~0x1F, 0x20, GetArchiveHeap());

	int ret = DVDReadPrio(&dvdhandle, handle->filePtr, (handle->length+0x1F) & ~0x1F, 0, 2);

	DVDClose(&dvdhandle);


	return handle->filePtr;
}

bool FreeFile(FileHandle *handle) {
	if (!handle) return false;

	if (handle->filePtr) {
		EGG__Heap__free(handle->filePtr, GetArchiveHeap());
	}

	handle->filePtr = 0;
	handle->length = 0;

	return true;
}




File::File() {
	m_loaded = false;
}

File::~File() {
	close();
}

bool File::open(const char *filename) {
	if (m_loaded)
		close();

	void *ret = LoadFile(&m_handle, filename);
	if (ret != 0)
		m_loaded = true;

	return (ret != 0);
}

/*bool File::openCompressed(const char *filename) {
	if (m_loaded)
		close();

	void *ret = LoadCompressedFile(&m_handle, filename);
	if (ret != 0)
		m_loaded = true;

	return (ret != 0);
}*/

void File::close() {
	if (!m_loaded)
		return;

	m_loaded = false;
	FreeFile(&m_handle);
}

bool File::isOpen() {
	return m_loaded;
}

void *File::ptr() {
	if (m_loaded)
		return m_handle.filePtr;
	else
		return 0;
}

u32 File::length() {
	if (m_loaded)
		return m_handle.length;
	else
		return 0xFFFFFFFF;
}


//
// processed\../src/levelinfo.cpp
//

#include "levelinfo.h"

dDvdLoader_c s_levelInfoLoader;
bool s_levelInfoLoaded = false;

dLevelInfo_c dLevelInfo_c::s_info;

bool LoadLevelInfo() {
	if (s_levelInfoLoaded)
		return true;

	void *data = s_levelInfoLoader.load("/NewerRes/LevelInfo.bin");
	if (data) {
		dLevelInfo_c::s_info.load(data);
		s_levelInfoLoaded = true;
		return true;
	}

	return false;
}


void dLevelInfo_c::load(void *buffer) {
	data = (header_s*)buffer;

	// decode all the level names
	for (int sect = 0; sect < sectionCount(); sect++) {
		// parse this section
		section_s *thisSect = getSectionByIndex(sect);

		for (int lev = 0; lev < thisSect->levelCount; lev++) {
			entry_s *level = &thisSect->levels[lev];

			if (level->levelSlot < 42)
				SetSomeConditionShit(level->worldSlot, level->levelSlot, level->flags);

			char *name = (char*)getNameForLevel(level);

			for (int i = 0; i < level->nameLength+1; i++) {
				name[i] -= 0xD0;
			}
		}
	}
}

dLevelInfo_c::entry_s *dLevelInfo_c::searchBySlot(int world, int level) {
	for (int i = 0; i < sectionCount(); i++) {
		section_s *sect = getSectionByIndex(i);

		for (int j = 0; j < sect->levelCount; j++) {
			entry_s *entry = &sect->levels[j];
			if (entry->worldSlot == world && entry->levelSlot == level)
				return entry;
		}
	}

	return 0;
}

dLevelInfo_c::entry_s *dLevelInfo_c::searchByDisplayNum(int world, int level) {
	for (int i = 0; i < sectionCount(); i++) {
		section_s *sect = getSectionByIndex(i);

		for (int j = 0; j < sect->levelCount; j++) {
			entry_s *entry = &sect->levels[j];
			if (entry->displayWorld == world && entry->displayLevel == level)
				return entry;
		}
	}

	return 0;
}


void UpdateFSStars() {
	dLevelInfo_c *li = &dLevelInfo_c::s_info;
	SaveBlock *save = GetSaveFile()->GetBlock(-1);

	bool coinsNormal = true, exitsNormal = true;
	bool coinsW9 = true, exitsW9 = true;

	for (int i = 0; i < li->sectionCount(); i++) {
		dLevelInfo_c::section_s *sect = li->getSectionByIndex(i);

		for (int j = 0; j < sect->levelCount; j++) {
			dLevelInfo_c::entry_s *entry = &sect->levels[j];

			// Levels only
			if (!(entry->flags & 2))
				continue;

			u32 conds = save->GetLevelCondition(entry->worldSlot, entry->levelSlot);

			if (entry->displayWorld == 9) {
				if ((conds & COND_COIN_ALL) != COND_COIN_ALL)
					coinsW9 = false;
				if (entry->flags & 0x10)
					if (!(conds & COND_NORMAL))
						exitsW9 = false;
				if (entry->flags & 0x20)
					if (!(conds & COND_SECRET))
						exitsW9 = false;
			} else {
				if ((conds & COND_COIN_ALL) != COND_COIN_ALL)
					coinsNormal = false;
				if (entry->flags & 0x10)
					if (!(conds & COND_NORMAL))
						exitsNormal = false;
				if (entry->flags & 0x20)
					if (!(conds & COND_SECRET))
						exitsNormal = false;
			}
		}
	}

	bool beatGame = (save->GetLevelCondition(7, 23) & COND_NORMAL) != 0;

//	save->bitfield &= ~0x3E;
	save->bitfield &= ~0x3C;
	save->bitfield |=
//		(beatGame ? 2 : 0) |
		(exitsNormal ? 4 : 0) |
		(coinsNormal ? 8 : 0) |
		(exitsW9 ? 0x10 : 0) |
		(coinsW9 ? 0x20 : 0);

	OSReport("FS Stars updated: Status: Game beaten: %d, Normal exits: %d, Normal coins: %d, W9 exits: %d, W9 coins: %d\n", beatGame, exitsNormal, coinsNormal, exitsW9, coinsW9);
}


//
// processed\../src/koopatlas/core.cpp
//

#include "koopatlas/core.h"
#include "koopatlas/camera.h"
#include "koopatlas/player.h"
#include "music.h"

extern "C" void LoadMapScene();

dScKoopatlas_c *dScKoopatlas_c::instance = 0;

CREATE_STATE_E(dScKoopatlas_c, Limbo);
CREATE_STATE(dScKoopatlas_c, ContinueWait);
CREATE_STATE_E(dScKoopatlas_c, Normal);
CREATE_STATE(dScKoopatlas_c, CompletionMsg);
CREATE_STATE_E(dScKoopatlas_c, CompletionMsgHideWait);
CREATE_STATE_E(dScKoopatlas_c, CSMenu);
CREATE_STATE_E(dScKoopatlas_c, TitleConfirmOpenWait);
CREATE_STATE_E(dScKoopatlas_c, TitleConfirmSelect);
CREATE_STATE_E(dScKoopatlas_c, TitleConfirmHitWait);
CREATE_STATE_E(dScKoopatlas_c, PlayerChangeWait);
CREATE_STATE_E(dScKoopatlas_c, EasyPairingWait);
CREATE_STATE_E(dScKoopatlas_c, PowerupsWait);
CREATE_STATE_E(dScKoopatlas_c, ShopWait);
CREATE_STATE_E(dScKoopatlas_c, CoinsWait);
CREATE_STATE_E(dScKoopatlas_c, SaveOpen);
CREATE_STATE_E(dScKoopatlas_c, SaveSelect);
CREATE_STATE_E(dScKoopatlas_c, SaveWindowClose);
CREATE_STATE_E(dScKoopatlas_c, SaveDo);
CREATE_STATE_E(dScKoopatlas_c, SaveEndWindow);
CREATE_STATE_E(dScKoopatlas_c, SaveEndCloseWait);
#ifdef QUICK_SAVE
CREATE_STATE_E(dScKoopatlas_c, QuickSaveOpen);
CREATE_STATE_E(dScKoopatlas_c, QuickSaveSelect);
CREATE_STATE_E(dScKoopatlas_c, QuickSaveWindowClose);
CREATE_STATE_E(dScKoopatlas_c, QuickSaveDo);
CREATE_STATE_E(dScKoopatlas_c, QuickSaveEndWindow);
CREATE_STATE_E(dScKoopatlas_c, QuickSaveEndCloseWait);
#endif
CREATE_STATE_E(dScKoopatlas_c, SaveError);

dScKoopatlas_c *dScKoopatlas_c::build() {
	// return new dScKoopatlas_c;
	MapReport("Creating WorldMap\n");

	void *buffer = AllocFromGameHeap1(sizeof(dScKoopatlas_c));
	dScKoopatlas_c *c = new(buffer) dScKoopatlas_c;

	MapReport("Created WorldMap @ %p\n", c);

	instance = c;
	return c;
}

bool WMInit_StartLoading(void*);
bool WMInit_LoadStaticFiles(void*);
bool StockWMInit_LoadEffects(void*);
bool WMInit_LoadSIAnims(void*);
bool WMInit_EndLoading(void*);
bool WMInit_LoadResources1(void*);
bool WMInit_LoadResources2(void*);
bool WMInit_SetupWait(void*);
bool WMInit_SetupExtra(void*);
bool WMInit_SetupWipe(void*);

ChainedFunc initFunctions[] = {
	WMInit_StartLoading,
	WMInit_LoadStaticFiles,
	StockWMInit_LoadEffects,
	WMInit_LoadSIAnims,
	WMInit_EndLoading,
	WMInit_LoadResources1,
	WMInit_LoadResources2,
	WMInit_SetupWait,
	WMInit_SetupExtra,
	WMInit_SetupWipe
};

dScKoopatlas_c::dScKoopatlas_c() : state(this) {
	initChain.setup(initFunctions, 9);

	setInitChain(initChain);
}



#define SELC_SETUP_DONE(sc) (*((bool*)(((u32)(sc))+0xD38)))

#define EASYP_SETUP_DONE(ep) (*((bool*)(((u32)(ep))+0x278)))
#define EASYP_ACTIVE(ep) (*((bool*)(((u32)(ep))+0x279)))

#define CSMENU_SETUP_DONE(csm) (*((bool*)(((u32)(csm))+0x270)))
#define CSMENU_ACTIVE(csm) (*((bool*)(((u32)(csm))+0x271)))
#define CSMENU_CHOICE_OK(csm) (*((bool*)(((u32)(csm))+0x272)))
#define CSMENU_UNK(csm) (*((bool*)(((u32)(csm))+0x273)))
#define CSMENU_CURRENT(csm) (*((int*)(((u32)(csm))+0x268)))

#define NPCHG_SETUP_DONE(npc) (*((bool*)(((u32)(npc))+0x67C)))
#define NPCHG_ACTIVE(npc) (*((bool*)(((u32)(npc))+0x67E)))
#define NPCHG_HIDE_FOR_EASYP(npc) (*((bool*)(((u32)(npc))+0x67F)))
#define NPCHG_READY(npc) (*((bool*)(((u32)(npc))+0x680)))
#define NPCHG_CCSB(npc,idx) (((void**)(((u32)(npc))+0x74))[(idx)])
#define NPCHG_CCSC(npc,idx) (((void**)(((u32)(npc))+0x84))[(idx)])
#define NPCHG_CCSA(npc,idx) (((void**)(((u32)(npc))+0x94))[(idx)])
#define NPCHG_CCI(npc,idx) (((void**)(((u32)(npc))+0xA4))[(idx)])
#define NPCHG_2DPLAYER(npc,idx) (((void**)(((u32)(npc))+0x64C))[(idx)])

#define CCSB_ACTIVE(ccsb) (*((bool*)(((u32)(ccsb))+0x29C)))

#define CCSC_ACTIVE(ccsc) (*((bool*)(((u32)(ccsc))+0x2A1)))

#define PLAYER2D_SHOW_EASY_PAIRING(p2d) (*((bool*)(((u32)(p2d))+0x264)))

#define CONT_LIVES(cont,idx) (((int*)(((u32)(cont))+0x2B8))[(idx)])
#define CONT_SETUP_DONE(cont) (*((bool*)(((u32)(cont))+0x2D4)))
#define CONT_UNK1(cont) (*((bool*)(((u32)(cont))+0x2D5)))
#define CONT_UNK2(cont) (*((bool*)(((u32)(cont))+0x2D6)))
#define CONT_DONE(cont) (*((bool*)(((u32)(cont))+0x2D7)))
#define CONT_UNK3(cont) (*((bool*)(((u32)(cont))+0x2E0)))

bool WMInit_StartLoading(void *ptr) {
	SpammyReport("WMInit_StartLoading called\n");

	DVD_Start();
	return true;
}

bool WMInit_LoadStaticFiles(void *ptr) {
	return dKPMusic::loadInfo();
}

bool WMInit_LoadSIAnims(void *ptr) {
	SpammyReport("WMInit_LoadSIAnims called\n");
	
	DVD_LoadFile(GetDVDClass(), "WorldMap", "SI_kinoko", 0);
	DVD_LoadFile(GetDVDClass(), "WorldMap", "SI_fireflower", 0);
	DVD_LoadFile(GetDVDClass(), "WorldMap", "SI_iceflower", 0);
	DVD_LoadFile(GetDVDClass(), "WorldMap", "SI_penguin", 0);
	DVD_LoadFile(GetDVDClass(), "WorldMap", "SI_propeller", 0);
	DVD_LoadFile(GetDVDClass(), "WorldMap", "SI_star", 0);
	DVD_LoadFile(GetDVDClass(), "Maps", "SI_hammer", 0);

	DVD_LoadFile(GetDVDClass(), "Object", "cobCourse", 0);
	DVD_LoadFile(GetDVDClass(), "Object", "I_kinoko_bundle", 0);
	DVD_LoadFile(GetDVDClass(), "Object", "lakitu", 0);
	DVD_LoadFile(GetDVDClass(), "Object", "star_coin", 0);
	DVD_LoadFile(GetDVDClass(), "Object", "StarRing", 0);
	return true;
}

bool WMInit_EndLoading(void *ptr) {
	SpammyReport("WMInit_EndLoading called\n");

	if (DVD_StillLoading(GetDVDClass2())) {
		SpammyReport("WMInit_EndLoading returning false\n");
		return false;
	}

	DVD_End();
	SpammyReport("WMInit_EndLoading returning true\n");
	return true;
}

bool WMInit_LoadResources1(void *ptr) {
	SpammyReport("WMInit_LoadResources1 returning true\n");

	dScKoopatlas_c *wm = (dScKoopatlas_c*)ptr;

	return wm->mapListLoader.load("/Maps/List.txt");
}

bool WMInit_LoadResources2(void *ptr) {
	SpammyReport("WMInit_LoadResources2 returning true\n");

	dScKoopatlas_c *wm = (dScKoopatlas_c*)ptr;

	if (wm->mapPath == 0) {
		wm->mapPath = wm->getMapNameForIndex(wm->currentMapID);
		if (wm->mapPath == 0)
			wm->mapPath = wm->getMapNameForIndex(0);
		if (!strcmp(wm->mapPath, "/Maps/WSEL.kpbin"))
			wm->warpZoneHacks = true;
		else
			wm->warpZoneHacks = false;
		OSReport("Load map: %s\n", wm->mapPath);
	}

	if (wm->mapData.load(wm->mapPath)) {
		return true;
	} else
		return false;
}

bool WMInit_SetupWait(void *ptr) {
	SpammyReport("WMInit_SetupWait called\n");

	dScKoopatlas_c *wm = (dScKoopatlas_c*)ptr;

	bool success = true;

	success &= CSMENU_SETUP_DONE(wm->csMenu);
	success &= SELC_SETUP_DONE(wm->selectCursor);
	success &= NPCHG_SETUP_DONE(wm->numPeopleChange);
	success &= wm->yesNoWindow->layoutLoaded;
	success &= CONT_SETUP_DONE(wm->continueObj);
	success &= wm->stockItem->layoutLoaded;
	success &= wm->stockItemShadow->layoutLoaded;
	success &= EASYP_SETUP_DONE(wm->easyPairing);

	return success;
}

bool WMInit_SetupExtra(void *ptr) {
	SpammyReport("WMInit_SetupExtra called\n");

	// ok, now we can set up other required shit
	dScKoopatlas_c *wm = (dScKoopatlas_c*)ptr;

	// check if we need to handle Continue
	if (CheckIfContinueShouldBeActivated()) {
		SpammyReport("continue is activated\n");
		wm->state.setState(&wm->StateID_ContinueWait);
	} else {
		SpammyReport("continue is not activated\n");
		wm->state.setState(&wm->StateID_Normal);
	}

	// first up: player models for Stocked Items
	for (int i = 0; i < 4; i++) {
		void *obj = CreateChildObject(WM_2D_PLAYER, wm, i, 0, 0);
		wm->stockItem->player2d[i] = obj;
		NPCHG_2DPLAYER(wm->numPeopleChange,i) = obj;
	}

	// next: items for the Powerup screen
	for (int i = 0; i < 8; i++) {
		void *obj = CreateChildObject(WM_ITEM, wm, i, 0, 0);
		wm->stockItem->newItemPtr[i] = obj;
	}

	// need Player before we can set up paths
	SpammyReport("creating player\n");
	wm->player = (daWMPlayer_c*)CreateParentedObject(WM_PLAYER, wm, 0, 2);
	wm->player->modelHandler->mdlClass->setPowerup(Player_Powerup[0]);
	wm->player->bindPats();
	wm->player->modelHandler->mdlClass->startAnimation(0, 1.2f, 10.0f, 0.0f);

	// since we've got all the resources, set up the path data too
	SpammyReport("preparing path manager\n");
	wm->pathManager.setup();

	// and put the player into position
	dKPNode_s *cNode = wm->pathManager.currentNode;
	wm->player->pos = (Vec){cNode->x, -cNode->y, wm->player->pos.z};

	// is last param correct? must check :/
	SpammyReport("creating MAP\n");
	wm->map = (dWMMap_c*)CreateParentedObject(WM_MAP, wm, 0, 0);
	SpammyReport("creating HUD\n");
	wm->hud = (dWMHud_c*)CreateParentedObject(WM_HUD, wm, 0, 0);
	// note: world_camera is not created here
	// because we require it earlier
	// it is created in dScKoopatlas_c::onCreate

	SpammyReport("creating SHOP\n");
	wm->shop = (dWMShop_c*)CreateParentedObject(WM_SHOP, wm, 0, 2);

	SpammyReport("creating Star Coin Menu\n");
	wm->coins = (dWMStarCoin_c*)CreateParentedObject(WM_STARCOIN, wm, 0, 0);


	SpammyReport("SetupExtra done\n");

	return true;
}

bool WMInit_SetupWipe(void *ptr) {
	SpammyReport("WMInit_SetupWipe called\n");

	dScKoopatlas_c *wm = (dScKoopatlas_c*)ptr;

	if (wm->hasUninitialisedProcesses()) {
		SpammyReport("WMInit_SetupWipe returning false\n");

#ifdef WM_SPAMMY_DEBUGGING
		fBase_c *p = wm->findNextUninitialisedProcess();
		SpammyReport("Not done yet: %p [%d]\n", p, p->name);
#endif
		return false;
	}

	SpammyReport("WMInit_SetupWipe returning true\n");

	return true;
}





/*void dScKoopatlas_c::startLevel(LevelInfo::Entry *entry) {
  for (int i = 0; i < 4; i++) {
  bool isThere = QueryPlayerAvailability(i);
  int id = Player_ID[i];
  Player_Active[i] = isThere ? 1 : 0;
  if (!isThere) Player_Flags[i] = 0;
  }

  StartLevelInfo sl;
  sl.unk1 = 0;
  sl.unk2 = 0xFF;
  sl.unk3 = 0;
  sl.unk4 = 0;
  sl.purpose = 0;

  sl.world1 = entry->world;
  sl.world2 = entry->world;
  sl.level1 = entry->level;
  sl.level2 = entry->level;

// hopefully this will fix the Star Coin issues
SetSomeConditionShit(entry->world, entry->level, 2);

ActivateWipe(WIPE_MARIO);

DoStartLevel(GetGameMgr(), &sl);
}*/

void dScKoopatlas_c::startMusic() {
	dKPMusic::play(GetSaveFile()->GetBlock(-1)->currentMapMusic);
}


int dScKoopatlas_c::onCreate() {
	OSReport("KP scene settings: %08x\n", settings);

	SpammyReport("onCreate() called\n");

	SpammyReport("Freeing effects\n"); // Opening cutscene loads vs effects for some reason and fragments RAM too much for some maps
	FreeEffects(0);
	FreeBreff(0);
	FreeBreft(0);

	SpammyReport("LoadMapScene()\n");
	LoadMapScene();

	SpammyReport("GameSetup__LoadScene(0)\n");
	GameSetup__LoadScene(0); // lol, stolen from GAME_SETUP

	SpammyReport("Setting Active Players\n");
	for (int i = 0; i < 4; i++) {
		bool isThere = QueryPlayerAvailability(i);
		int id = Player_ID[i];
		Player_Active[i] = isThere ? 1 : 0;
		if (!isThere) Player_Flags[i] = 0;
	}

	SpammyReport("select cursor\n");
	Actors aSelectCursor = translateActorID(SELECT_CURSOR);
	this->selectCursor = CreateParentedObject(aSelectCursor, this, 0, 0);

	SpammyReport("cs menu\n");
	this->csMenu = CreateParentedObject(COURSE_SELECT_MENU, this, 0, 0);

	SpammyReport("yes no window\n");
	Actors aYesNoWindow = translateActorID(YES_NO_WINDOW);
	this->yesNoWindow = (dYesNoWindow_c*)CreateParentedObject(aYesNoWindow, this, 0, 0);

	SpammyReport("number of people change\n");
	this->numPeopleChange = CreateParentedObject(NUMBER_OF_PEOPLE_CHANGE, this, 0, 0);

	for (int i = 0; i < 4; i++) {
		SpammyReport("ccsb %d\n", i);
		Actors aCharacterChangeSelectBase = translateActorID(CHARACTER_CHANGE_SELECT_BASE);
		void *ccsb = CreateParentedObject(aCharacterChangeSelectBase, this, i, 0);
		
		SpammyReport("ccsc %d\n", i);
		Actors aCharacterChangeSelectContents = translateActorID(CHARACTER_CHANGE_SELECT_CONTENTS);
		void *ccsc = CreateParentedObject(aCharacterChangeSelectContents, this, i, 0);
		
		SpammyReport("ccsa %d\n", i);
		Actors aCharacterChangeSelectArrow = translateActorID(CHARACTER_CHANGE_SELECT_ARROW);
		void *ccsa = CreateParentedObject(aCharacterChangeSelectArrow, this, i, 0);
		
		SpammyReport("ccsi %d\n", i);
		Actors aCharacterChangeSelectIndicator = translateActorID(CHARACTER_CHANGE_INDICATOR);
		void *cci = CreateParentedObject(aCharacterChangeSelectIndicator, this, i, 0);

		NPCHG_CCSB(this->numPeopleChange, i) = ccsb;
		NPCHG_CCSC(this->numPeopleChange, i) = ccsc;
		NPCHG_CCSA(this->numPeopleChange, i) = ccsa;
		NPCHG_CCI(this->numPeopleChange, i) = cci;
	}

	SpammyReport("continue\n");
	Actors aContinue = translateActorID(CONTINUE);
	this->continueObj = CreateParentedObject(aContinue, this, 0, 0);

	SpammyReport("stock item\n");
	this->stockItem = (dStockItem_c*)CreateParentedObject(STOCK_ITEM, this, 0, 0);
	SpammyReport("stock item shadow\n");
	this->stockItemShadow = (dStockItemShadow_c*)CreateParentedObject(STOCK_ITEM_SHADOW, this, 0, 0);
	stockItem->shadow = stockItemShadow;

	SpammyReport("easy pairing\n");
	this->easyPairing = CreateParentedObject(EASY_PAIRING, this, 0, 0);

	SpammyReport("world camera\n");
	CreateParentedObject(WORLD_CAMERA, this, 0, 0);

	SpammyReport("setting NewerMapDrawFunc\n");
	*CurrentDrawFunc = NewerMapDrawFunc;

	SpammyReport("onCreate() completed\n");
	
	// Prepare this first
	SaveBlock *save = GetSaveFile()->GetBlock(-1);
	currentMapID = save->current_world;
	isFirstPlay = (currentMapID == 0) && (settings & 0x80000000);

	// Are we coming from Kamek cutscene? If so, then do.. some stuff!
	isAfterKamekCutscene = (settings & 0x40000000);
	if (isAfterKamekCutscene) {
		currentMapID = 6; // KoopaPlanet
		save->current_world = 6;
	}

	if (MaybeFinishingLevel[0] == 7 && MaybeFinishingLevel[1] == 24 && save->CheckLevelCondition(7, 24, COND_NORMAL)) {
		currentMapID = 7; // KoopaPlanetUnd
		save->current_world = 7;
		isAfter8Castle = true;
	}

	isEndingScene = (settings & 0x20000000);
	if (isEndingScene) {
		currentMapID = 0;
		save->current_world = 0;
		save->current_path_node = 0;
	}

	somethingAboutSound(_8042A788);

	return true;
}

extern "C" void PlaySoundWithFunctionB4(void *spc, nw4r::snd::SoundHandle *handle, int id, int unk);

int dScKoopatlas_c::onDelete() {
	if (!keepMusicPlaying)
		dKPMusic::stop();

	FreeScene(0);
	FreeScene(1);

	CleanUpEffectThings();
	FreeEffects(1);
	FreeBreff(1);
	FreeBreft(1);

	DVD_FreeFile(GetDVDClass2(), "SI_kinoko");
	DVD_FreeFile(GetDVDClass2(), "SI_fireflower");
	DVD_FreeFile(GetDVDClass2(), "SI_iceflower");
	DVD_FreeFile(GetDVDClass2(), "SI_penguin");
	DVD_FreeFile(GetDVDClass2(), "SI_propeller");
	DVD_FreeFile(GetDVDClass2(), "SI_star");
	DVD_FreeFile(GetDVDClass2(), "SI_hammer");

	DVD_FreeFile(GetDVDClass2(), "cobCourse");
	DVD_FreeFile(GetDVDClass2(), "I_kinoko_bundle");
	DVD_FreeFile(GetDVDClass2(), "lakitu");
	DVD_FreeFile(GetDVDClass2(), "star_coin");
	DVD_FreeFile(GetDVDClass2(), "StarRing");

	mapListLoader.unload();

	CleanUpEffectThings();

	return true;
}

bool dScKoopatlas_c::canDoStuff() {
	if (QueryGlobal5758(0xFFFFFFFF)) return false;
	if (CheckIfWeCantDoStuff()) return false;
	if (state.getCurrentState() == &StateID_Limbo) return false;
	return true;
}

bool dScKoopatlas_c::mapIsRunning() {
	if (QueryGlobal5758(0xFFFFFFFF)) return false;
	if (CheckIfWeCantDoStuff()) return false;
	if (state.getCurrentState() != &StateID_Normal) return false;
	return true;
}


int dScKoopatlas_c::onExecute() {
	dKPMusic::execute();
	if (!canDoStuff()) return true;

	//SpammyReport("Executing state: %s\n", state.getCurrentState()->getName());
	state.execute();

	return true;
}


void dScKoopatlas_c::executeState_Limbo() { }


void dScKoopatlas_c::beginState_ContinueWait() {
	GameMgrP->_AFC = 1;
	CONT_UNK1(this->continueObj) = true;
	CONT_UNK2(this->continueObj) = true;
	CONT_UNK3(this->continueObj) = false;
}
void dScKoopatlas_c::executeState_ContinueWait() {
	// Waiting for the Continue anim to finish
	if (CONT_DONE(this->continueObj)) {
		CONT_UNK1(this->continueObj) = 0;
		CONT_UNK2(this->continueObj) = 0;
		CONT_UNK3(this->continueObj) = 0;

		static const int things[] = {0,1,3,2,4};
		for (int i = 0; i < 4; i++) {
			int idx = SearchForIndexOfPlayerID(things[i]);
			Player_Lives[Player_ID[idx]] = CONT_LIVES(this->continueObj, i);
		}

		state.setState(&StateID_Normal);
	}
}
void dScKoopatlas_c::endState_ContinueWait() {
	GameMgrP->_AFC = 0;
}


void dScKoopatlas_c::executeState_Normal() {
	if (pathManager.completionMessagePending) {
		OSReport("Going to set CompletionMsg\n");
		state.setState(&StateID_CompletionMsg);
		return;
	}

	if (pathManager.doingThings())
		return;

	int nowPressed = Remocon_GetPressed(GetActiveRemocon());

	// Nothing related to the menu is going on
	if (nowPressed & WPAD_ONE) {
		stockItem->show = true;
		state.setState(&StateID_PowerupsWait);
		hud->hideAll();
	} else if (nowPressed & WPAD_PLUS) {
		CSMENU_ACTIVE(this->csMenu) = true;
		state.setState(&StateID_CSMenu);
		hud->hideAll();
#ifdef NEWER_DEBUG
	 } else if (nowPressed & WPAD_MINUS) {
	 	pathManager.unlockAllPaths(2);
	 } else if (nowPressed & WPAD_A) {
	 	pathManager.unlockAllPaths(0);
	 	SaveBlock *save = GetSaveFile()->GetBlock(-1);
	 	for (int w = 0; w < 6; w++)
	 		for (int l = 0; l < 6; l++)
	 			save->SetLevelCondition(w, l, COND_COIN_ALL);
#endif
	} 
}

void dScKoopatlas_c::executeState_CSMenu() {
	// The course select menu is currently being shown

	// First off, check to see if it's been hidden
	if (!CSMENU_ACTIVE(this->csMenu)) {
		// That means something happened
		if (CSMENU_CHOICE_OK(this->csMenu)) {
			// Player pressed a button
			SaveBlock *save = GetSaveFile()->GetBlock(-1);

			switch (CSMENU_CURRENT(this->csMenu)) {
				case 0:
					// Star Coins
					coins->show();
					state.setState(&StateID_CoinsWait);
					break;

				case 1:
					// Add/Drop Players
					MapReport("Add/Drop Players was pressed\n");
					state.setState(&StateID_PlayerChangeWait);
					NPCHG_ACTIVE(this->numPeopleChange) = true;
					WpadShit(10);

					break;

				case 2:
					// Save or Quick Save
					MapReport("Save or Quick Save was pressed\n");
#ifdef QUICK_SAVE
					if (GetSaveFile()->GetBlock(-1)->bitfield & 2) {
#endif
						state.setState(&StateID_SaveOpen);
						yesNoWindow->type = 1;
						yesNoWindow->visible = true;

#ifdef QUICK_SAVE
					} else {
						state.setState(&StateID_QuickSaveOpen);
						yesNoWindow->type = 15;
						yesNoWindow->visible = 1;

					}
#endif
					break;

				case 3:
					// Title Screen
					MapReport("Title Screen was pressed\n");
					state.setState(&StateID_TitleConfirmOpenWait);
					yesNoWindow->visible = true;
					yesNoWindow->type = 10;
					break;
			}

		} else {
			// Ok, change back to STATE_Normal
			hud->unhideAll();
			state.setState(&StateID_Normal);
		}
	}
}

void dScKoopatlas_c::executeState_TitleConfirmOpenWait() {
	// Waiting for the "Go to Title Screen" YesNoWindow to finish opening

	if (!yesNoWindow->animationActive) {
		state.setState(&StateID_TitleConfirmSelect);
	}

}

/**********************************************************************/
// STATE_TitleConfirmSelect : Let the user choose an option on the
// 	"Go to Title Screen" YesNoWindow.
void dScKoopatlas_c::executeState_TitleConfirmSelect() {
	int nowPressed = Remocon_GetPressed(GetActiveRemocon());

	if (nowPressed & WPAD_LEFT) {
		// Select "OK!"
		yesNoWindow->current = 1;

	} else if (nowPressed & WPAD_RIGHT) {
		// Select "Cancel"
		yesNoWindow->current = 0;

	} else if (Wiimote_TestButtons(GetActiveWiimote(), WPAD_A | WPAD_TWO)) {
		// Pick the current option
		yesNoWindow->close = true;
		if (yesNoWindow->current != 1)
			yesNoWindow->keepOpen = true;
		state.setState(&StateID_TitleConfirmHitWait);

	} else {
		// Cancel using B or 1
		if (CheckIfMenuShouldBeCancelledForSpecifiedWiimote(0)) {
			yesNoWindow->cancelled = true;
			yesNoWindow->current = true;
			state.setState(&StateID_TitleConfirmHitWait);
		}
	}

}

/**********************************************************************/
// STATE_TitleConfirmHitWait : Process the user's chosen option on
// 	the "Go to Title Screen" YesNoWindow. Also, wait for the
//  animation to be complete.
void dScKoopatlas_c::executeState_TitleConfirmHitWait() {

	if (!yesNoWindow->animationActive) {
		if (yesNoWindow->current == 1) {
			state.setState(&StateID_Normal);
			hud->unhideAll();
		} else {
			state.setState(&StateID_Limbo);
			StartTitleScreenStage(false, 0);
		}
	}

}

/**********************************************************************/
// STATE_PlayerChangeWait : Wait for the user to do something on the
// 	Add/Drop Players screen.
void dScKoopatlas_c::executeState_PlayerChangeWait() {
	int nowPressed = Remocon_GetPressed(GetActiveRemocon());

	if (NPCHG_READY(this->numPeopleChange)) {
		if (nowPressed & WPAD_PLUS) {
			// activate easy pairing. FUN !!
			NPCHG_HIDE_FOR_EASYP(this->numPeopleChange) = 1;

			for (int i = 0; i < 4; i++) {
				void *obj = NPCHG_2DPLAYER(this->numPeopleChange, i);
				void *ccsb = NPCHG_CCSB(this->numPeopleChange, i);
				void *ccsc = NPCHG_CCSC(this->numPeopleChange, i);

				PLAYER2D_SHOW_EASY_PAIRING(obj) = 1;
				CCSB_ACTIVE(ccsb) = 1;
				CCSC_ACTIVE(ccsc) = 1;
			}

			EASYP_ACTIVE(this->easyPairing) = 1;
			state.setState(&StateID_EasyPairingWait);
		}
	} else {
		if (!NPCHG_ACTIVE(this->numPeopleChange)) {

			for (int i = 0; i < 4; i++) {
				bool isThere = QueryPlayerAvailability(i);
				int id = Player_ID[i];
				Player_Active[i] = isThere ? 1 : 0;
				if (!isThere) Player_Flags[i] = 0;
			}

			state.setState(&StateID_Normal);
			hud->unhideAll();
		}
	}

}

/**********************************************************************/
// STATE_EasyPairingWait : Wait for the user to exit Easy Pairing.
void dScKoopatlas_c::executeState_EasyPairingWait() {

	if (!EASYP_ACTIVE(this->easyPairing)) {
		NPCHG_HIDE_FOR_EASYP(this->numPeopleChange) = 0;

		for (int i = 0; i < 4; i++) {
			void *obj = NPCHG_2DPLAYER(this->numPeopleChange, i);
			void *ccsb = NPCHG_CCSB(this->numPeopleChange, i);
			void *ccsc = NPCHG_CCSC(this->numPeopleChange, i);

			PLAYER2D_SHOW_EASY_PAIRING(obj) = 0;
			CCSB_ACTIVE(ccsb) = 0;
			CCSC_ACTIVE(ccsc) = 0;
		}

		state.setState(&StateID_PlayerChangeWait);
		WpadShit(10);
	}

}

/**********************************************************************/
// STATE_PowerupsWait : Wait for the user to exit the Powerups screen.
void dScKoopatlas_c::executeState_PowerupsWait() {

	if (!stockItem->show) {
		player->modelHandler->mdlClass->setPowerup(Player_Powerup[0]);
		player->bindPats();

		state.setState(&StateID_Normal);
		hud->unhideAll();
	}

}


/**********************************************************************/
// STATE_ShopWait : Wait for the user to exit the Shop screen.
void dScKoopatlas_c::executeState_ShopWait() {

	if (!shop->visible) {
		state.setState(&StateID_Normal);
		hud->unhideAll();
	}

}


/**********************************************************************/
// STATE_StarCoin : Wait for the user to exit the Star Coin screen.
void dScKoopatlas_c::executeState_CoinsWait() {

	if (!coins->visible) {
		state.setState(&StateID_Normal);
		hud->unhideAll();
	}

}

/**********************************************************************/
// STATE_SaveOpen : Waiting for the "Save?" YesNoWindow to open
void dScKoopatlas_c::executeState_SaveOpen() {

	if (!yesNoWindow->animationActive) {
		state.setState(&StateID_SaveSelect);
	}

}

/**********************************************************************/
// STATE_SaveSelect : Let the user choose an option on the
// 	"Save?" YesNoWindow.
void dScKoopatlas_c::executeState_SaveSelect() {
	int nowPressed = Remocon_GetPressed(GetActiveRemocon());

	if (nowPressed & WPAD_LEFT) {
		// Select "OK!"
		yesNoWindow->current = 1;

	} else if (nowPressed & WPAD_RIGHT) {
		// Select "Cancel"
		yesNoWindow->current = 0;

	} else if (Wiimote_TestButtons(GetActiveWiimote(), WPAD_A | WPAD_TWO)) {
		// Pick the current option
		yesNoWindow->close = true;

		if (yesNoWindow->current != 1)
			yesNoWindow->hasBG = true;
		state.setState(&StateID_SaveWindowClose);

	} else {
		// Cancel using B or 1
		if (CheckIfMenuShouldBeCancelledForSpecifiedWiimote(0)) {
			yesNoWindow->cancelled = true;
			yesNoWindow->current = 1;
			state.setState(&StateID_SaveWindowClose);
		}
	}

}

/**********************************************************************/
// STATE_SaveWindowClose : Process the user's chosen option on the
// 	"Save?" YesNoWindow. Also, wait for the animation to be complete.
void dScKoopatlas_c::executeState_SaveWindowClose() {

	if (!yesNoWindow->visible) {
		if (yesNoWindow->current == 1) {
			state.setState(&StateID_Normal);
			hud->unhideAll();
		} else {
			state.setState(&StateID_SaveDo);
			SaveGame(0, false);
		}
	}

}

/**********************************************************************/
// STATE_SaveDo : Save the game.
void dScKoopatlas_c::executeState_SaveDo() {

	if (!GetSaveFile()->CheckIfWriting()) {
		if (GetSaveHandler()->CurrentError == 0) {
			yesNoWindow->type = 2;
			yesNoWindow->visible = true;
			state.setState(&StateID_SaveEndWindow);
		} else {
			state.setState(&StateID_SaveError);
		}
	}

}

/**********************************************************************/
// STATE_SaveEndWindow : Handle the Save End window.
void dScKoopatlas_c::executeState_SaveEndWindow() {

	if (!yesNoWindow->animationActive) {
		if (Wiimote_TestButtons(GetActiveWiimote(), WPAD_A | WPAD_TWO)) {
			yesNoWindow->close = true;
			state.setState(&StateID_SaveEndCloseWait);
		}
	}

}

/**********************************************************************/
// STATE_SaveEndCloseWait : Wait for the Save End window to close.
void dScKoopatlas_c::executeState_SaveEndCloseWait() {

	if (!yesNoWindow->animationActive) {
		state.setState(&StateID_Normal);
		hud->unhideAll();
	}

}

#ifdef QUICK_SAVE
/**********************************************************************/
// STATE_QuickSaveOpen : Waiting for the "Save?" YesNoWindow to open
void dScKoopatlas_c::executeState_QuickSaveOpen() {

	if (!yesNoWindow->animationActive) {
		state.setState(&StateID_QuickSaveSelect);
	}

}

/**********************************************************************/
// STATE_QuickSaveSelect : Let the user choose an option on the
// 	"Save?" YesNoWindow.
void dScKoopatlas_c::executeState_QuickSaveSelect() {
	int nowPressed = Remocon_GetPressed(GetActiveRemocon());

	if (nowPressed & WPAD_LEFT) {
		// Select "OK!"
		yesNoWindow->current = 1;

	} else if (nowPressed & WPAD_RIGHT) {
		// Select "Cancel"
		yesNoWindow->current = 0;

	} else if (Wiimote_TestButtons(GetActiveWiimote(), WPAD_A | WPAD_TWO)) {
		// Pick the current option
		yesNoWindow->close = true;

		if (yesNoWindow->current != 1)
			yesNoWindow->hasBG = true;
		state.setState(&StateID_QuickSaveWindowClose);

	} else {
		// Cancel using B or 1
		if (CheckIfMenuShouldBeCancelledForSpecifiedWiimote(0)) {
			yesNoWindow->cancelled = true;
			yesNoWindow->current = 1;
			state.setState(&StateID_QuickSaveWindowClose);
		}
	}

}

/**********************************************************************/
// STATE_QuickSaveWindowClose : Process the user's chosen option on
// 	the "Save?" YesNoWindow. Also, wait for the animation to be complete
void dScKoopatlas_c::executeState_QuickSaveWindowClose() {

	if (!yesNoWindow->visible) {
		if (yesNoWindow->current == 1) {
			state.setState(&StateID_Normal);
			hud->unhideAll();
		} else {
			state.setState(&StateID_QuickSaveDo);
			SaveGame(0, true);
		}
	}

}

/**********************************************************************/
// STATE_QuickSaveDo : Save the game.
void dScKoopatlas_c::executeState_QuickSaveDo() {

	if (!GetSaveFile()->CheckIfWriting()) {
		if (GetSaveHandler()->CurrentError == 0) {
			yesNoWindow->type = 16;
			yesNoWindow->visible = true;
			state.setState(&StateID_QuickSaveEndWindow);
		} else {
			state.setState(&StateID_SaveError);
		}
	}

}

// STATE_QuickSaveEndWindow : Handle the Save End window.
void dScKoopatlas_c::executeState_QuickSaveEndWindow() {

	if (!yesNoWindow->animationActive) {
		if (Wiimote_TestButtons(GetActiveWiimote(), WPAD_A | WPAD_TWO)) {
			yesNoWindow->close = true;
			yesNoWindow->keepOpen = true;
			state.setState(&StateID_QuickSaveEndCloseWait);
		}
	}

}

void dScKoopatlas_c::executeState_QuickSaveEndCloseWait() {
	// Wait for Save End window to close

	if (!yesNoWindow->animationActive) {
		if (yesNoWindow->current == 1) {
			state.setState(&StateID_Normal);
			hud->unhideAll();
		} else {
			state.setState(&StateID_Limbo);
			StartTitleScreenStage(false, 0);
		}
	}

}
#endif


void dScKoopatlas_c::executeState_SaveError() { }


void dScKoopatlas_c::startLevel(dLevelInfo_c::entry_s *entry) {

	StartLevelInfo sl;
	sl.replayTypeMaybe = 0;
	sl.entrance = 0xFF;
	sl.areaMaybe = 0;
	sl.unk4 = 0;
	sl.purpose = 0;

	sl.world1 = entry->worldSlot;
	sl.world2 = entry->worldSlot;
	sl.level1 = entry->levelSlot;
	sl.level2 = entry->levelSlot;

	ActivateWipe(WIPE_MARIO);

	DoStartLevel(GetGameMgr(), &sl);
}


u32 dScKoopatlas_c::iterateMapList(u32(*callback)(u32,const char *,int,int), u32 userData, int *ptrIndex) {
	u8 *ptr = (u8*)mapListLoader.buffer;
	u8 *strStart = ptr;
	u8 *end = ptr + mapListLoader.size;
	int index = 0;

	while (true) {
		u8 chr = *ptr;
		if (chr == 13) {
			*ptr = 0;
			++ptr;
			continue;
		}

		if (chr == 10 || chr == 0 || ptr >= end) {
			if (strStart == ptr) {
				// Blank string, ignore
				++strStart;
				++ptr;
				continue;
			}

			// Change the linefeed to a NUL so we can use the line as a C string later
			if (ptr < end)
				*ptr = 0;

			u32 ret = callback(userData, (const char*)strStart, ptr - strStart, index);
			if (ptrIndex)
				*ptrIndex = index;
			if (ret > 0)
				return ret;

			strStart = ++ptr;
			++index;

			if (ptr >= end)
				break;

		} else {
			++ptr;
		}
	}

	return 0;
}

static u32 _cb_getIndex(u32 userData, const char *str, int size, int index) {
	if (index == userData)
		return (u32)str;
	else
		return 0;
}

static u32 _cb_searchName(u32 userData, const char *str, int size, int index) {
	if (strncmp(str, (const char*)userData, size) == 0)
		return (u32)(index+1);
	else
		return 0;
}

const char *dScKoopatlas_c::getMapNameForIndex(int index) {
	return (const char *)iterateMapList(&_cb_getIndex, (u32)index, 0);
}

int dScKoopatlas_c::getIndexForMapName(const char *name) {
	return ((int)iterateMapList(&_cb_searchName, (u32)name, 0)) - 1;
}


void dScKoopatlas_c::showSaveWindow() {
	hud->hideAll();
	state.setState(&StateID_SaveOpen);
	yesNoWindow->type = 1;
	yesNoWindow->visible = true;
}

static const wchar_t *completionMsgs[] = {
	L"The most erudite of Buttocks",
	L"You've collected all of\nthe \x0B\x014F\xBEEF Star Coins in\n",
#ifdef FALLING_LEAF
	L"You've gotten all of the\nexits in Newer Falling Leaf!",
	L"You've done everything there\nis to do in the game!\nYou're a super player!\nThanks for playing!",
#else
	L"You have gotten every \x0B\x013B\xBEEF exit\nin",
	L"You have gotten everything\nin",
#endif
	L"You have collected all the\nnecessary \x0B\x014F\xBEEF coins to enter\nthe Special World!",
	L"You have collected all the \x0B\x014F\xBEEF Star\nCoins in the game!",
	L"You've found every \x0B\x013B\xBEEF exit in the\ngame!",
	L"You've completed everything in\nNEWER SUPER MARIO BROS. Wii!\n\nWe present to you a new quest.\nTry pressing \x0B\x0122\xBEEF and \x0B\x0125\xBEEF\n on the Star Coin menu."
};

void dScKoopatlas_c::beginState_CompletionMsg() {
	OSReport("CompletionMsg beginning with type %d\n", pathManager.completionMessageType);
	static const int ynTypes[8] = {
		/*NULL*/ -1,
		/*COINS*/ 14,
		/*EXITS*/ 7,
		/*WORLD*/ 8,
		/*COINS EXC W9*/ 9,
		/*GLOBAL COINS*/ 11,
		/*GLOBAL EXITS*/ 27,
		/*EVERYTHING*/ 21
	};
	yesNoWindow->type = ynTypes[pathManager.completionMessageType];
	yesNoWindow->visible = true;
	mustFixYesNoText = 10; // hacky shit
}

void dScKoopatlas_c::endState_CompletionMsg() {
	pathManager.completionMessagePending = false;
	pathManager.completionMessageType = 0;
}

void dScKoopatlas_c::executeState_CompletionMsg() {
	// hacky shit
	if (mustFixYesNoText > 0) {
		mustFixYesNoText--;

		int type = pathManager.completionMessageType;

		const wchar_t *baseText = completionMsgs[type];
		// Used when we assemble a dynamic message
		wchar_t text[512];

#ifndef FALLING_LEAF
		if (type >= CMP_MSG_COINS && type <= CMP_MSG_WORLD) {
			// title
			int w = pathManager.completionMessageWorldNum;
			int l = ((w == 5) || (w == 7)) ? 101 : 100;
			dLevelInfo_c::entry_s *titleEntry = dLevelInfo_c::s_info.searchByDisplayNum(w, l);
			const char *title = dLevelInfo_c::s_info.getNameForLevel(titleEntry);

			// assemble the string

			wcscpy(text, baseText);
			int pos = wcslen(text);

			text[pos++] = ' ';

			while (*title) {
				char chr = *(title++);
				if (chr != '-')
					text[pos++] = chr;
			}

			text[pos++] = '!';
			text[pos++] = 0;
			baseText = text;
		}
#endif

		yesNoWindow->T_question_00->SetString(baseText);
		yesNoWindow->T_questionS_00->SetString(baseText);
	}

	if (!yesNoWindow->animationActive) {
		if (Wiimote_TestButtons(GetActiveWiimote(), WPAD_A | WPAD_TWO)) {
			yesNoWindow->close = true;
			state.setState(&StateID_CompletionMsgHideWait);
		}
	}
}

void dScKoopatlas_c::executeState_CompletionMsgHideWait() {
	if (!yesNoWindow->visible)
		state.setState(&StateID_Normal);
}


void NewerMapDrawFunc() {
	Reset3DState();
	SetCurrentCameraID(0);
	DoSomethingCameraRelatedWithEGGScreen(0, &dWorldCamera_c::instance->screen);
	LinkScene(0);
	SceneCalcWorld(0);
	SceneCameraStuff(0);
	ChangeAlphaUpdate(false);
	DrawOpa();
	DrawXlu();
	UnlinkScene(0);
	// Something
	SetupLYTDrawing();
	DrawAllLayoutsBeforeX(0x81);
	RenderEffects(0, 3);
	RenderEffects(0, 2);
	GXDrawDone();
	RemoveAllFromScnRoot();
	Reset3DState();
	SetCurrentCameraID(1);
	DoSpecialDrawing1();
	LinkScene(1);
	SceneCalcWorld(1);
	SceneCameraStuff(1);
	CalcMaterial();
	DrawOpa();
	DrawXlu();
	UnlinkScene(1);
	SetCurrentCameraID(0);
	for (int i = 0; i < 4; i++)
		RenderEffects(0, 0xB+i);
	for (int i = 0; i < 4; i++)
		RenderEffects(0, 7+i);
	GXDrawDone();
	// Leaving out some stuff here
	DrawAllLayoutsAfterX(0x80);
	ClearLayoutDrawList();
	SetCurrentCameraID(0);
}


//
// processed\../src/koopatlas/player.cpp
//

#include "koopatlas/player.h"

daWMPlayer_c *daWMPlayer_c::instance;
static const char *mdlNames[] = {"MB_model", "SMB_model", "PLMB_model", "PMB_model"};
static const char *patNames[] = {"PB_switch_swim", "PB_switch_swim", "PLMB_switch_swim", "PLB_switch_swim"};

int daWMPlayer_c::onCreate() {

	this->modelHandler = new dPlayerModelHandler_c(0);
	// loadModel(u8 player_id, int powerup_id, int unk);
	// Unk is some kind of mode: 0=in-game, 1=map, 2=2D

	// Here we do a bit of a hack
	//this->modelHandler->loadModel(0, 3, 1);
	dPlayerModel_c *pm = (dPlayerModel_c*)modelHandler->mdlClass;

	pm->mode_maybe = 1;
	pm->player_id_1 = 0;
	pm->allocator.link(0xC000, GameHeaps[0], 0, 0x20);
	pm->prepare();

	for (int i = 0; i < 4; i++) {
		nw4r::g3d::ResMdl mdl = pm->modelResFile.GetResMdl(mdlNames[i]);
		nw4r::g3d::ResAnmTexPat pat = pm->modelResFile.GetResAnmTexPat(patNames[i]);

		pats[i].setup(mdl, pat, &pm->allocator, 0, 1);
	}

	pm->allocator.unlink();
	pm->setPowerup(3);
	pm->finaliseModel();

	pm->startAnimation(0, 1.2, 10.0, 0.0);
	modelHandler->setSRT((Vec){0.0,100.0,-100.0}, (S16Vec){0,0,0}, (Vec){2.0,2.0,2.0});

	hammerSuit.setup(this->modelHandler);

	pos = (Vec){0.0f,0.0f,3000.0f};
	rot = (S16Vec){0x1800,0,0};
	scale = (Vec){1.6f,1.6f,1.6f};

	hasEffect = false;
	hasSound = false;
	step = false;
	effectName = "";
	soundName = 0;
	timer = 0;
	jumpOffset = 0.0;

	// -1 or 0xC000? normally we use -1 but Player uses 0xC000....
	//allocator.link(0xC000, GameHeaps[0], 0, 0x20);
	//allocator.unlink();

	return true;
}

int daWMPlayer_c::onDelete() {
	delete modelHandler;

	return true;
}


int daWMPlayer_c::onExecute() {
	if (Player_Flags[0] & 1) {
		modelHandler->mdlClass->enableStarColours();
		modelHandler->mdlClass->enableStarEffects();
		dKPMusic::playStarMusic();
	}

	if (spinning)
		rot.y += 0xC00;
	else
		SmoothRotation(&rot.y, targetRotY, 0xC00);

	if (dScKoopatlas_c::instance->mapIsRunning())
		dScKoopatlas_c::instance->pathManager.execute();

	this->modelHandler->update();
	pats[((dPlayerModel_c*)modelHandler->mdlClass)->currentPlayerModelID].process();

	mMtx myMatrix;
	myMatrix.scale(scale.x, scale.y, scale.z);
	myMatrix.applyTranslation(pos.x, pos.y + jumpOffset, pos.z);
	if (dScKoopatlas_c::instance->warpZoneHacks && (currentAnim == jump || currentAnim == jumped))
		myMatrix.applyTranslation(0, 0, 600.0f);
	myMatrix.applyRotationX(&rot.x);
	myMatrix.applyRotationY(&rot.y);
	// Z is unused for now
	modelHandler->setMatrix(myMatrix);

	if (dScKoopatlas_c::instance->mapIsRunning()) {
		if (hasEffect) { 
			Vec effPos = {pos.x, pos.y, 3300.0f};
			effect.spawn(effectName, 0, &effPos, &rot, &scale);
		}

		if (hasSound) {
			timer++;

			if (timer == 12) {
				if (step) { MapSoundPlayer(SoundRelatedClass, soundName, 1); step = false; }
				else { MapSoundPlayer(SoundRelatedClass, soundName+1, 1); step = true; }
				timer = 0;
			}

			if (timer > 12) { timer = 0; }
		}
	}

	return true;
}

int daWMPlayer_c::onDraw() {
	if (!visible)
		return true;
	if (dScKoopatlas_c::instance->isEndingScene)
		return true;

	this->modelHandler->draw();
	hammerSuit.draw();

	return true;
}


void daWMPlayer_c::startAnimation(int id, float frame, float unk, float updateRate) {
	if (id == currentAnim && frame == currentFrame && unk == currentUnk && updateRate == currentUpdateRate)
		return;

	bool isOldSwimming = (currentAnim == swim_wait);
	bool isNewSwimming = (id == swim_wait);

	currentAnim = id;
	currentFrame = frame;
	currentUnk = unk;
	currentUpdateRate = updateRate;
	this->modelHandler->mdlClass->startAnimation(id, frame, unk, updateRate);

	if (isOldSwimming != isNewSwimming)
		bindPats();
}

void daWMPlayer_c::bindPats() {
	dPlayerModel_c *pm = (dPlayerModel_c*)modelHandler->mdlClass;
	int id = pm->currentPlayerModelID;

	static const float frames[] = {0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 2.0f, 3.0f};
	float frame = frames[pm->powerup_id];
	if (currentAnim == swim_wait)
		frame += (pm->powerup_id == 4 || pm->powerup_id == 5) ? 1.0f : 4.0f;

	nw4r::g3d::ResAnmTexPat pat = pm->modelResFile.GetResAnmTexPat(patNames[id]);
	pats[id].bindEntry(
			&pm->models[id].body,
			&pat,
			0, 4);
	pats[id].setUpdateRateForEntry(0.0f, 0);
	pats[id].setFrameForEntry(frame, 0);

	pm->models[id].body.bindAnim(&pats[id]);
}


daWMPlayer_c *daWMPlayer_c::build() {

	void *buffer = AllocFromGameHeap1(sizeof(daWMPlayer_c));
	daWMPlayer_c *c = new(buffer) daWMPlayer_c;


	instance = c;
	return c;
}


//
// processed\../src/koopatlas/hud.cpp
//

#include "koopatlas/hud.h"
#include <newer.h>

dWMHud_c *dWMHud_c::instance = 0;

dWMHud_c *dWMHud_c::build() {

	void *buffer = AllocFromGameHeap1(sizeof(dWMHud_c));
	dWMHud_c *c = new(buffer) dWMHud_c;


	instance = c;
	return c;
}

dWMHud_c::dWMHud_c() {
	layoutLoaded = false;
	displayedControllerType = -1;
	isFooterVisible = false;
}

enum WMHudAnimation {
	SHOW_LIVES = 0,
	SHOW_HEADER,
	SHOW_FOOTER,
	HIDE_ALL,
	UNHIDE_ALL,
};


int dWMHud_c::onCreate() {
	if (!layoutLoaded) {
		bool gotFile = layout.loadArc("MapHUD.arc", false);
		if (!gotFile)
			return false;

		bool output = layout.build("maphud.brlyt");

		layout.layout.rootPane->trans.x = -112.0f;
		if (IsWideScreen()) {
			layout.layout.rootPane->scale.x = 0.735f;
		} else {
			layout.clippingEnabled = true;
			layout.clipX = 0;
			layout.clipY = 52;
			layout.clipWidth = 640;
			layout.clipHeight = 352;
			layout.layout.rootPane->scale.x = 0.731f;
			layout.layout.rootPane->scale.y = 0.7711f;
			layout.drawInfo._50 &= ~0x20;
		}

		static const char *brlanNames[] = {
			"MapHUD_ShowMain.brlan", "MapHUD_ShowHeader.brlan",
			"MapHUD_HideAll.brlan", "MapHUD_UnhideAll.brlan",
		};
		static const char *groupNames[] = {
			"G_Lives", "G_Header", "G_Footer",
			"G_Hideables", "G_Hideables",
		};

		layout.loadAnimations(brlanNames, 4);
		layout.loadGroups(groupNames, (int[5]){0, 1, 0, 2, 3}, 5);
		layout.disableAllAnimations();

		layout.enableNonLoopAnim(SHOW_LIVES);
		layout.resetAnim(SHOW_FOOTER);
		layout.resetAnim(SHOW_HEADER);
		layout.resetAnim(HIDE_ALL);

		static const char *tbNames[2] = {"MenuButtonInfo", "ItemsButtonInfo"};
		layout.setLangStrings(tbNames, (int[2]){12, 15}, 4, 2);

		static const char *paneNames[] = {
			"N_IconPos1P_00", "N_IconPos2P_00",
			"N_IconPos3P_00", "N_IconPos4P_00"
		};
		layout.getPanes(paneNames, &N_IconPosXP_00[0], 4);

		static const char *pictureNames[] = {
			"Header_Centre", "Header_Right", "Footer",
			"NormalExitFlag", "SecretExitFlag",
			"StarCoinOff0", "StarCoinOff1", "StarCoinOff2",
			"StarCoinOn0", "StarCoinOn1", "StarCoinOn2",
			"P_marioFace_00", "P_luigiFace_00",
			"P_BkinoFace_00", "P_YkinoFace_00",
			"Star0", "Star1", "Star2"
		};
		layout.getPictures(pictureNames, &Header_Centre, 18);

		static const char *textBoxNames[] = {
			"LevelName", "LevelNameS",
			"LevelNumber", "LevelNumberS",
			"WorldName", "WorldNameS",
			"StarCoinCounter",
			"T_lifeNumber_00", "T_lifeNumber_01",
			"T_lifeNumber_02", "T_lifeNumber_03"
		};
		layout.getTextBoxes(textBoxNames, &LevelName, 11);

		headerCol.setTexMap(Header_Right->material->texMaps);
		headerCol.applyAlso(Header_Centre->material->texMaps);
		footerCol.setTexMap(Footer->material->texMaps);

		layoutLoaded = true;

		layout.drawOrder = 0;

		willShowHeader = false;
		willShowFooter = false;

		loadFooterInfo();

		setupLives();
	}

	return true;
}


void dWMHud_c::loadInitially() {
	if (doneFirstShow)
		return;

	doneFirstShow = true;

	SaveBlock *save = GetSaveFile()->GetBlock(-1);
	willShowFooter = (save->newerWorldName[0] != 0) && (save->hudHintH != 2000);

	if (!dScKoopatlas_c::instance->pathManager.isMoving)
		enteredNode();
}


int dWMHud_c::onDelete() {
	dWMHud_c::instance = 0;

	if (!layoutLoaded)
		return true;

	return layout.free();
}


int dWMHud_c::onExecute() {
	if (!layoutLoaded)
		return true;

	if (willShowHeader && (!(layout.isAnimOn(SHOW_HEADER)))) {
		willShowHeader = false;
		loadHeaderInfo();
		playShowAnim(SHOW_HEADER);
	}

	if (willShowFooter && (!(layout.isAnimOn(SHOW_FOOTER)))) {
		willShowFooter = false;
		isFooterVisible = true;
		loadFooterInfo();
		playShowAnim(SHOW_FOOTER);
	}

	setupLives(); // FUCK IT
	updatePressableButtonThingies();

	int scCount = getUnspentStarCoinCount();
	WriteNumberToTextBox(&scCount, StarCoinCounter, false);

	layout.execAnimations();
	layout.update();

	return true;
}


int dWMHud_c::onDraw() {
	if (!layoutLoaded)
		return true;

	layout.scheduleForDrawing();

	return true;
}


void dWMHud_c::hideAll() {
	if (!layout.isAnimOn(HIDE_ALL))
		layout.enableNonLoopAnim(HIDE_ALL);
	layout.grpHandlers[HIDE_ALL].frameCtrl.flags = 1; // NO_LOOP
}
void dWMHud_c::unhideAll() {
	if (!layout.isAnimOn(HIDE_ALL))
		layout.enableNonLoopAnim(HIDE_ALL, true);
	layout.grpHandlers[HIDE_ALL].frameCtrl.flags = 3; // NO_LOOP | REVERSE
}




void dWMHud_c::playShowAnim(int id) {
	if (!this || !this->layoutLoaded) return;

	layout.enableNonLoopAnim(id);
}

void dWMHud_c::playHideAnim(int id) {
	if (!this || !this->layoutLoaded) return;

	if (!layout.isAnimOn(id)) {
		layout.enableNonLoopAnim(id, true);
	}
	layout.grpHandlers[id].frameCtrl.flags = 3; // NO_LOOP | REVERSE
	if (id == SHOW_FOOTER)
		isFooterVisible = false;
}


void dWMHud_c::loadHeaderInfo() {
	dLevelInfo_c *levelInfo = &dLevelInfo_c::s_info;

	dLevelInfo_c::entry_s *infEntry = levelInfo->searchBySlot(
			nodeForHeader->levelNumber[0]-1, nodeForHeader->levelNumber[1]-1);

	if (infEntry == 0) {
		LevelName->SetString(L"Unknown Level Name!");
		LevelNameS->SetString(L"Unknown Level Name!");
		return;
	}

	// LEVEL NAME
	wchar_t convertedLevelName[100];
	const char *sourceLevelName = levelInfo->getNameForLevel(infEntry);
	int charCount = 0;
	
	while (*sourceLevelName != 0 && charCount < 99) {
		convertedLevelName[charCount] = *sourceLevelName;
		sourceLevelName++;
		charCount++;
	}
	convertedLevelName[charCount] = 0;

	LevelName->SetString(convertedLevelName);
	LevelNameS->SetString(convertedLevelName);

	// a hack because I don't feel like editing the rlyt
	LevelName->size.x = LevelNameS->size.x = 400.0f;

	// LEVEL NUMBER
	wchar_t levelNumber[16];
	getNewerLevelNumberString(infEntry->displayWorld, infEntry->displayLevel, levelNumber);

	LevelNumber->SetString(levelNumber);

	// make the picture shadowy
	int sidx = 0;
	while (levelNumber[sidx]) {
		if (levelNumber[sidx] == 11) {
			levelNumber[sidx+1] = 0x200 | (levelNumber[sidx+1]&0xFF);
			sidx += 2;
		}
		sidx++;
	}
	LevelNumberS->SetString(levelNumber);

	nw4r::ut::TextWriter tw2;
	tw2.font = LevelNumber->font;
	tw2.SetFontSize(LevelNumber->fontSizeX, LevelNumber->fontSizeY);
	tw2.lineSpace = LevelNumber->lineSpace;
	tw2.charSpace = LevelNumber->charSpace;
	if (LevelNumber->tagProc != 0)
		tw2.tagProcessor = LevelNumber->tagProc;

	float currentPos = tw2.CalcStringWidth(levelNumber, wcslen(levelNumber));
	currentPos += LevelNumber->trans.x + 12.0f;

	// INFO
	int w = nodeForHeader->levelNumber[0] - 1;
	int l = nodeForHeader->levelNumber[1] - 1;

	u32 conds = GetSaveFile()->GetBlock(-1)->GetLevelCondition(w, l);
	// States: 0 = invisible, 1 = visible, 2 = faded
	int normalState = 0, secretState = 0;

	if ((conds & COND_BOTH_EXITS) && (infEntry->flags & 0x30) == 0x30) {
		// If this level has two exits and one of them is already collected,
		// then show the faded flags
		normalState = 2;
		secretState = 2;
	}

	if ((conds & COND_NORMAL) && (infEntry->flags & 0x10))
		normalState = 1;
	if ((conds & COND_SECRET) && (infEntry->flags & 0x20))
		secretState = 1;

	NormalExitFlag->trans.x = currentPos;
	NormalExitFlag->alpha = (normalState == 2) ? 80 : 255;
	NormalExitFlag->SetVisible(normalState > 0);
	if (normalState > 0)
		currentPos += NormalExitFlag->size.x;

	SecretExitFlag->trans.x = currentPos;
	SecretExitFlag->alpha = (secretState == 2) ? 80 : 255;
	SecretExitFlag->SetVisible(secretState > 0);
	if (secretState > 0)
		currentPos += SecretExitFlag->size.x;

	// are star coins enabled or not?
	bool haveSC = (infEntry->flags & 2);

	for (int i = 0; i < 3; i++) {
		bool flag = (conds & (COND_COIN1 << i));
		StarCoinOn[i]->SetVisible(flag);
		StarCoinOff[i]->SetVisible(haveSC);
		if (haveSC) {
			StarCoinOff[i]->trans.x = currentPos;
			currentPos += StarCoinOff[i]->size.x + 4.0f;
		}
	}

	// SIZE THING
	nw4r::ut::TextWriter tw;
	tw.font = LevelName->font;
	tw.SetFontSize(LevelName->fontSizeX, LevelName->fontSizeY);
	tw.lineSpace = LevelName->lineSpace;
	tw.charSpace = LevelName->charSpace;
	if (LevelName->tagProc != 0)
		tw.tagProcessor = LevelName->tagProc;

	float width = tw.CalcStringWidth(convertedLevelName, charCount);
	float totalWidth = width + LevelName->trans.x - 20.0f;
	if (totalWidth < currentPos)
		totalWidth = currentPos;
	Header_Centre->size.x = totalWidth;
	Header_Right->trans.x = totalWidth;

	SaveBlock *save = GetSaveFile()->GetBlock(-1);
	headerCol.colourise(save->hudHintH%1000, save->hudHintS, save->hudHintL);
}


void dWMHud_c::loadFooterInfo() {
	SaveBlock *save = GetSaveFile()->GetBlock(-1);

	wchar_t convertedWorldName[32];
	int i;
	for (i = 0; i < 32; i++) {
		convertedWorldName[i] = save->newerWorldName[i];
		if (convertedWorldName[i] == 0)
			break;
	}
	convertedWorldName[31] = 0;

	WorldName->SetString(convertedWorldName);
	WorldNameS->SetString(convertedWorldName);

	WorldName->colour1 = save->hudTextColours[0];
	WorldName->colour2 = save->hudTextColours[1];

	footerCol.colourise(save->hudHintH%1000, save->hudHintS, save->hudHintL);

	// figure out if stars are needed
	// Star 0: world is complete
	// Star 1: all exits complete
	// Star 2: all star coins obtained
	
	bool starVisibility[3];
	starVisibility[0] = false;

	dLevelInfo_c *linfo = &dLevelInfo_c::s_info;
	dLevelInfo_c::entry_s *lastLevel = linfo->searchByDisplayNum(save->newerWorldID, lastLevelIDs[save->newerWorldID]);
	if (lastLevel) {
		starVisibility[0] = (save->GetLevelCondition(lastLevel->worldSlot,lastLevel->levelSlot) & COND_NORMAL);
	}

	// now calculate the other two
	starVisibility[1] = true;
	starVisibility[2] = true;

	dLevelInfo_c::section_s *sect = linfo->getSectionByIndex(save->newerWorldID);

	for (int i = 0; i < sect->levelCount; i++) {
		dLevelInfo_c::entry_s *entry = &sect->levels[i];

		if (entry->flags & 2) {
			u32 conds = save->GetLevelCondition(entry->worldSlot, entry->levelSlot);

			if (((entry->flags & 0x10) && !(conds & COND_NORMAL)) ||
					((entry->flags & 0x20) && !(conds & COND_SECRET)))
						starVisibility[1] = false;

			if ((conds & COND_COIN_ALL) != COND_COIN_ALL)
				starVisibility[2] = false;
		}
	}

	if (save->newerWorldID == 15) {
		starVisibility[0] = false;
		starVisibility[1] = false;
		starVisibility[2] = false;
	}

	float startX = Star[0]->trans.x;
	for (int i = 0; i < 3; i++) {
		Star[i]->SetVisible(starVisibility[i]);
		Star[i]->trans.x = startX;
		if (starVisibility[i]) {
			startX += Star[i]->size.x + 4.0f;
		}
	}

	WorldName->trans.x = startX + 4.0f;
	WorldNameS->trans.x = startX + 6.0f;
}



void dWMHud_c::enteredNode(dKPNode_s *node) {
	if (node == 0)
		node = dScKoopatlas_c::instance->pathManager.currentNode;

	if (node->type == dKPNode_s::LEVEL && doneFirstShow) {
		willShowHeader = true;
		nodeForHeader = node;
	}
}

void dWMHud_c::leftNode() {
	if (layout.grpHandlers[SHOW_HEADER].frameCtrl.currentFrame > 0.1f) {
		// not hidden

		if ((layout.isAnimOn(SHOW_HEADER) && !(layout.grpHandlers[SHOW_HEADER].frameCtrl.flags & 2))
				|| (!layout.isAnimOn(SHOW_HEADER))) {
			// currently being shown, OR fully shown already
			playHideAnim(SHOW_HEADER);
		}
	}
}


void dWMHud_c::hideFooter() {
	if (isFooterVisible)
		playHideAnim(SHOW_FOOTER);
}

void dWMHud_c::showFooter() {
	if (!doneFirstShow)
		return;
	willShowFooter = true;
	if (isFooterVisible)
		playHideAnim(SHOW_FOOTER);
}


void dWMHud_c::setupLives() {
	static const int LogicalPlayerIDs[] = {0,1,3,2};

	P_marioFace_00->SetVisible(false);
	P_luigiFace_00->SetVisible(false);
	P_BkinoFace_00->SetVisible(false);
	P_YkinoFace_00->SetVisible(false);

	int playerCount = 0;

	for (int i = 0; i < 4; i++) {
		// The part in setupLives()
		int playerID = LogicalPlayerIDs[i];
		int slotID = SearchForIndexOfPlayerID(playerID);
		int lives = Player_Lives[slotID];
		int length = 2;

		WriteNumberToTextBox(&lives, &length, T_lifeNumber[slotID], true);

		// The part in setupIconThings()
		if (QueryPlayerAvailability(slotID)) {
			playerCount++;

			nw4r::lyt::Pane *facePane = (&P_marioFace_00)[playerID];
			facePane->trans = N_IconPosXP_00[playerCount - 1]->trans;
			facePane->SetVisible(true);
		}
	}

	for (int i = 0; i < 4; i++)
		N_IconPosXP_00[i]->SetVisible(false);
	N_IconPosXP_00[playerCount - 1]->SetVisible(true);
}

void dWMHud_c::updatePressableButtonThingies() {
	int cntType = RemoconMng->controllers[0]->controllerType;

	if (cntType != displayedControllerType) {
		displayedControllerType = cntType;

		int beef = (cntType == 0) ? 0 : 1;
		GameMgrP->currentControllerType = beef;

		WriteBMGToTextBox(
				layout.findTextBoxByName("ItemsButtonInfo"),
				GetBMG(), 4, 15, 0);
	}
}



//
// processed\../src/koopatlas/camera.cpp
//

#include "koopatlas/camera.h"
#include "koopatlas/player.h"

dWorldCamera_c *dWorldCamera_c::instance = 0;

dWorldCamera_c *dWorldCamera_c::build() {

	void *buffer = AllocFromGameHeap1(sizeof(dWorldCamera_c));
	dWorldCamera_c *c = new(buffer) dWorldCamera_c;


	instance = c;
	return c;
}


dWorldCamera_c::dWorldCamera_c() {
	camera3d.camPos = (Vec){0.0, 10.0, 0.0};
	camera3d.target = (Vec){0.0, 0.0, 0.0};
	camera3d.camUp = (Vec){0.0, 1.0, 0.0};

	camera2d.camPos = (Vec){0.0, 10.0, 0.0};
	camera2d.target = (Vec){0.0, 0.0, 0.0};
	camera2d.camUp = (Vec){0.0, 1.0, 0.0};

	camPos = (Vec){0.0, 0.0, 1.0};
	camUp = (Vec){0.0, 1.0, 0.0};

	projection2d.near = -20000.0f;
	projection2d.far = 20000.0f;

	currentX = 416;
	currentY = -224;
	zoomLevel = STD_ZOOM;

	followPlayer = true;
}



int dWorldCamera_c::onCreate() {
	// Bad Code
	screen.projType = 0;
	screen.some_flag_bit |= 1;

	SSM *ssm = &ScreenSizesAndMultipliers[currentScreenSizeID];
	screen.height = ssm->height;
	screen.width = ssm->width;
	screen.near = 1.0;
	screen.far = 20000.0;

	if (screen.isCentered != 1) {
		screen.some_flag_bit |= 1;
		screen.isCentered = 1;
	}

	doStuff(10000.0);
	generateCameraMatrices();
	updateCameras();

	return true;
}


int dWorldCamera_c::onDelete() {
	return true;
}


int dWorldCamera_c::onExecute() {
	if (dScKoopatlas_c::instance->warpZoneHacks) {
		currentX = 2040.0f;
		currentY = -1460.0f;
		zoomLevel = 3.4f;

	} else if (panning) {
		// Calculate where we are
#define SMOOTHSTEP(x) ((x) * (x) * (3 - 2 * (x)))
		float stepRatio = panCurrentStep / panTotalSteps;
		stepRatio = 1.0f - SMOOTHSTEP(stepRatio);
		//OSReport("PAN: Step %f / %f ---- Ratio: %f", panCurrentStep, panTotalSteps, stepRatio);
		//OSReport("From %f, %f to %f, %f --- Zoom: %f to %f\n", panFromX, panFromY, panToX, panToY, panFromZoom, panToZoom);

		currentX = (panFromX * stepRatio) + (panToX * (1.0f - stepRatio));
		currentY = (panFromY * stepRatio) + (panToY * (1.0f - stepRatio));
		zoomLevel = (panFromZoom * stepRatio) + (panToZoom * (1.0f - stepRatio));
		//OSReport("Calculated: %f, %f with zoom %f\n", currentX, currentY, zoomLevel);

		panCurrentStep += 1.0f;

		if (panCurrentStep > panTotalSteps) {
			// YAY, we reached the end
			panning = false;
			currentX = panToX;
			currentY = panToY;
			zoomLevel = panToZoom;
		}

	} else if (followPlayer) {
		daWMPlayer_c *player = daWMPlayer_c::instance;
		currentX = player->pos.x;
		currentY = player->pos.y;
	}

	calculateScreenGeometry();
	doStuff(10000.0);
	generateCameraMatrices();
	updateCameras(); 
	return true;
}

void dWorldCamera_c::panToBounds(float left, float top, float right, float bottom) {
	// Pad it a bit
	left -= 64.0f;
	right += 64.0f;
	top -= 48.0f;
	bottom += 48.0f;

	//OSReport("Panning camera to bounds %f,%f to %f,%f\n", left, top, right, bottom);

	// Figure out the centre x/y we want
	float width = right - left;
	float height = bottom - top;

	float desiredCentreX = left + (width * 0.5f);
	float desiredCentreY = -(top + (height * 0.5f));

	//OSReport("Size: %f x %f ; Desired Centre: %f, %f\n", width, height, desiredCentreX, desiredCentreY);

	// Our default zoom is 2.8
	float minScreenWidth = GlobalScreenWidth * 1.2f;
	float minScreenHeight = GlobalScreenHeight * 1.2f;
	float maxScreenWidth = GlobalScreenWidth * 4.0f;
	float maxScreenHeight = GlobalScreenHeight * 4.0f;

	//OSReport("Screen Sizes: Minimum possible %f x %f ; Maximum possible %f x %f\n", minScreenWidth, minScreenHeight, maxScreenWidth, maxScreenHeight);

	// First off, gotta cap it to the ratio
	float screenRatio = GlobalScreenWidth / GlobalScreenHeight;
	float boundsRatio = width / height;
	float correctedWidth = width, correctedHeight = height;
	float desiredZoomLevel;
	//OSReport("Actual screen size is %f x %f --- Screen Ratio: %f, Bounds Ratio: %f\n", GlobalScreenWidth, GlobalScreenHeight, screenRatio, boundsRatio);

	float widthScale = width / GlobalScreenWidth;
	float heightScale = height / GlobalScreenHeight;

	if (heightScale > widthScale) {
		// Thing is constrained on the top/bottom
		desiredZoomLevel = heightScale;
	} else {
		// Thing is constrained on the left/right
		desiredZoomLevel = widthScale;
	}

	//OSReport("Desired zoom level is %f\n", desiredZoomLevel);

	// Cap the zoom
	if (desiredZoomLevel < 2.0f)
		desiredZoomLevel = 2.0f;
	if (desiredZoomLevel > 4.5f)
		desiredZoomLevel = 4.5f;
	//OSReport("After capping: %f\n", desiredZoomLevel);

	// And we're almost there YAY
	panToPosition(desiredCentreX, desiredCentreY, desiredZoomLevel);
}


void dWorldCamera_c::panToPosition(float x, float y, float zoom) {
	panFromX = currentX;
	panFromY = currentY;
	panFromZoom = zoomLevel;

	panToX = x;
	panToY = y;
	panToZoom = zoom;

	float xDiff = abs(panToX - panFromX);
	float yDiff = abs(panToY - panFromY);

	float panLength = sqrtf((xDiff*xDiff) + (yDiff*yDiff));
	float panSteps = panLength / 2.3f;
	float scaleSteps = abs(panToZoom - panFromZoom) / 0.1f;
	float stepCount = max(panSteps, scaleSteps);

	//OSReport("Pan length: %f over %f steps\n", panLength, panSteps);
	//OSReport("Scale steps: %f\n", scaleSteps);
	//OSReport("Step Count: %f\n", stepCount);

	panCurrentStep = 0.0f;
	panTotalSteps = stepCount;

	panning = true;
	followPlayer = false;
}


int dWorldCamera_c::onDraw() {
	GXRModeObj *rmode = nw4r::g3d::G3DState::GetRenderModeObj();

	nw4r::g3d::Camera cam(GetCameraByID(0));
	nw4r::g3d::Camera cam2d(GetCameraByID(1));

	if (rmode->field_rendering != 0) {
		cam.SetViewportJitter(VIGetNextField());
		cam2d.SetViewportJitter(VIGetNextField());
	}
	//cam2d.SetOrtho(rmode->efbHeight, 0.0f, 0.0f, rmode->fbWidth * (IsWideScreen() ? 1.3333334f : 1.0f), -100000.0f, 100000.0f);

	return true;
}


void dWorldCamera_c::calculateScreenGeometry() {
	zoomDivisor = 1.0 / zoomLevel;

	screenWidth = GlobalScreenWidth / zoomDivisor;
	screenHeight = GlobalScreenHeight / zoomDivisor;

	screenLeft = currentX - (screenWidth * 0.5);
	screenTop = currentY + (screenHeight * 0.5);
}


void dWorldCamera_c::doStuff(float something) {
	camTarget = (Vec){
		screenLeft + (screenWidth * 0.5),
	   (screenTop - screenHeight) + (screenHeight * 0.5),
	   0.0
	};

	camPos = (Vec){camTarget.x, camTarget.y, something};
}


void dWorldCamera_c::generateCameraMatrices() {
	float orthoTop = screenHeight * 0.5;
	float orthoLeft = -screenWidth * 0.5;
	float orthoBottom = -orthoTop;
	float orthoRight = -orthoLeft;

	camera3d.camPos = camPos;
	camera3d.target = camTarget;
	camera3d.camUp = camUp;

	camera3d.calculateMatrix();

	screen.setOrtho(orthoTop, orthoBottom, orthoLeft, orthoRight, -100000.0f, 100000.0f);

	VEC2 crap = GetSomeSizeRelatedBULLSHIT();
	Vec calcedAboutRatio = CalculateSomethingAboutRatio(orthoTop, orthoBottom, orthoLeft, orthoRight);
	screen._44 = CalculateSomethingElseAboutRatio();

	screen.width = crap.x;
	screen.some_flag_bit |= 1;
	screen.height = crap.y;
	screen.horizontalMultiplier = calcedAboutRatio.x;
	screen.verticalMultiplier = calcedAboutRatio.y;
	screen.unk3 = calcedAboutRatio.z;

	camera2d.camPos = (Vec){0.0, 0.0, 0.0};
	camera2d.target = (Vec){0.0, 0.0, -100.0};
	camera2d.camUp = camUp;

	camera2d.calculateMatrix();

	GXRModeObj *rmode = nw4r::g3d::G3DState::GetRenderModeObj();
	projection2d.setVolume(rmode->efbHeight, 0.0, 0.0, rmode->fbWidth * (IsWideScreen() ? 1.3333334f : 1.0f));
}


void dWorldCamera_c::updateCameras() {
	nw4r::g3d::Camera cam0(GetCameraByID(0));
	camera3d.assignToNW4RCamera(cam0);
	screen.loadIntoCamera(cam0);

	nw4r::g3d::Camera cam1(GetCameraByID(1));
	projection2d.setOrthoOntoCamera(cam1);
	camera2d.assignToNW4RCamera(cam1);
}


//
// processed\../src/koopatlas/map.cpp
//

#include "koopatlas/map.h"
#include "koopatlas/camera.h"

//#define TILE_DEBUGGING
//#define BOUND_DEBUGGING
//#define DOODAD_DEBUGGING

#ifdef TILE_DEBUGGING
#define TileReport OSReport
#else
#define TileReport(...)
#endif

#ifdef BOUND_DEBUGGING
#define BoundReport OSReport
#else
#define BoundReport(...)
#endif

#ifdef DOODAD_DEBUGGING
#define DoodadReport OSReport
#else
#define DoodadReport(...)
#endif

dWMMap_c *dWMMap_c::instance = 0;

dWMMap_c *dWMMap_c::build() {

	void *buffer = AllocFromGameHeap1(sizeof(dWMMap_c));
	dWMMap_c *c = new(buffer) dWMMap_c;


	instance = c;
	return c;
}



dWMMap_c::dWMMap_c() {
}


int dWMMap_c::onCreate() {
	renderer.allocator.setup(GameHeaps[0], 0x20);
	bool result = renderer.setup(&renderer.allocator);

	bgMatrix.translation(0.0f, 0.0f, -500.0f);

	allocator.link(-1, GameHeaps[0], 0, 0x20);
	nw4r::g3d::ResFile rf(dScKoopatlas_c::instance->mapData.bgLoader.buffer);
	rf.CheckRevision();
	rf.Init();
	rf.Bind(rf);

	// Todo: Check flags. 0x32C definitely supports texsrt
	nw4r::g3d::ResMdl modelRes = rf.GetResMdl("KpBG");
	nw4r::g3d::ResAnmTexSrt anmRes = rf.GetResAnmTexSrt("KpBG");

	bgModel.setup(modelRes, &allocator, 0x32C, 1, 0);
	bgSrtAnm.setup(modelRes, anmRes, &allocator, 0, 1);
	bgSrtAnm.bindEntry(&bgModel, anmRes, 0, 0);
	bgModel.bindAnim(&bgSrtAnm, 0.0f);

	static int EffectGroups[] = {1,0,4,5,7,8,9,10,11,12,13,14};
	static int EffectPrios[] = {141,142,143,129,144,145,146,147,148,149,150,151};
	for (int i = 0; i < EFFECT_RENDERER_COUNT; i++) {
		effectRenderers[i].setupEffectRenderer(&allocator, -1, EffectPrios[i], EffectGroups[i]);
	}

	bool lsRotate = false;
	switch (dScKoopatlas_c::instance->currentMapID) {
		case 5: // Starry Sky
			launchStarX = 5424.0f; launchStarY = -4416.0f; showLaunchStar = true; break;
		// case 11: // Sky City
		// 	launchStarX = 3216.0f; launchStarY = -3120.0f; showLaunchStar = true; break;
		case 6: // Planet Koopa
			lsRotate = true;
			launchStarX = 2268.0f; launchStarY = -3420.0f; showLaunchStar = true; break;
	}
	if (showLaunchStar) {
		nw4r::g3d::ResFile lsRes(getResource("StarRing", "g3d/StarRing.brres"));
		nw4r::g3d::ResMdl lsMdl = lsRes.GetResMdl("StarRing");
		launchStarModel.setup(lsMdl, &allocator, 0x32C, 1, 0);

		nw4r::g3d::ResAnmChr lsAnm = lsRes.GetResAnmChr("StarRing_wait");
		launchStarAnm.setup(lsMdl, lsAnm, &allocator, 0);

		launchStarAnm.bind(&launchStarModel, lsAnm, false);
		launchStarModel.bindAnim(&launchStarAnm);

		launchStarMatrix.translation(launchStarX, launchStarY, 1000.0f);
		S16Vec lsRot = {0x2000, lsRotate ? 0x6200 : -0x5C00, 0};
		launchStarMatrix.applyRotationYXZ(&lsRot.x, &lsRot.y, &lsRot.z);
	}

	allocator.unlink();

	return true;
}

int dWMMap_c::onDelete() { return true; }
int dWMMap_c::onExecute() {
	bgSrtAnm.process();

	bgModel.setDrawMatrix(bgMatrix);
	bgModel.setScale(1.0f, 1.0f, 1.0f);
	bgModel.calcWorld(false);

	doEffects();

	if (showLaunchStar) {
		launchStarAnm.process();
		launchStarModel._vf1C();
		launchStarModel.setDrawMatrix(launchStarMatrix);
		launchStarModel.setScale(0.035f, 0.035f, 0.035f);
		launchStarModel.calcWorld(false);
	}

	return true;
}

int dWMMap_c::onDraw() {

	renderer.scheduleForDrawing();
	bgModel.scheduleForDrawing();

	if (showLaunchStar)
		launchStarModel.scheduleForDrawing();

	for (int i = 0; i < EFFECT_RENDERER_COUNT; i++) {
		effectRenderers[i].scheduleForDrawing();
	}


	dKPMapData_c *dataCls = &dScKoopatlas_c::instance->mapData;
	renderPathLayer(dataCls->pathLayer);

	return true;
}




void dWMMap_c::renderer_c::drawOpa() {
	//drawLayers();
}

void dWMMap_c::renderer_c::drawXlu() {
	drawLayers();
}



void dWMMap_c::renderer_c::drawLayers() {
	dScKoopatlas_c *wm = dScKoopatlas_c::instance;
	dKPMapData_c *dataCls = &wm->mapData;
	dKPMapFile_s *data = dataCls->data;

	baseZ = -100.0f - (2 * data->layerCount);

	bool skipFirstLayer = (wm->currentMapID == 0) && !(wm->isFirstPlay);

	beginRendering();

	for (int iLayer = data->layerCount - 1; iLayer >= 0; iLayer--) {
		if (skipFirstLayer && iLayer == 0)
			continue;

		dKPLayer_s *layer = data->layers[iLayer];
		renderMtx[2][3] += 2.0f;

		if (layer->type == dKPLayer_s::PATHS) {
			// rebase the camera matrix
			baseZ = 3500.0f;
			nw4r::g3d::Camera cam3d(GetCameraByID(0));
			cam3d.GetCameraMtx(&renderMtx);
			MTXTransApply(renderMtx, renderMtx, 0, 0, baseZ);
		}

		if (layer->alpha == 0)
			continue; // invisible

		TileReport("Checking layer %d with type %d\n", iLayer, layer->type);

		GXColor whichCol = (GXColor){255,255,255,layer->alpha};
		GXSetTevColor(GX_TEVREG0, whichCol);

		if (layer->type == dKPLayer_s::OBJECTS) 	
			renderTileLayer(layer, data->sectors);
		else if (layer->type == dKPLayer_s::DOODADS)
			renderDoodadLayer(layer);
	}

	endRendering();
}

void dWMMap_c::renderer_c::beginRendering() {
	currentTexture = 0;

	nw4r::g3d::Camera cam3d(GetCameraByID(0));
	cam3d.GetCameraMtx(&renderMtx);
	MTXTransApply(renderMtx, renderMtx, 0, 0, baseZ);

	GXSetCurrentMtx(GX_PNMTX0);

	dWorldCamera_c *camObj = dWorldCamera_c::instance;
	minX = ((int)camObj->screenLeft) / 24;
	minY = ((int)(-camObj->screenTop) - 23) / 24;
	maxX = (((int)(camObj->screenLeft + camObj->screenWidth)) + 23) / 24;
	maxY = ((int)(-camObj->screenTop + camObj->screenHeight)) / 24;

	GXClearVtxDesc();

	GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
	GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);

	// Tiles
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XY, GX_S16, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);

	// Doodads
	GXSetVtxAttrFmt(GX_VTXFMT1, GX_VA_POS, GX_POS_XY, GX_F32, 0);
	GXSetVtxAttrFmt(GX_VTXFMT1, GX_VA_TEX0, GX_TEX_ST, GX_U8, 8);

	GXSetNumIndStages(0);
	for (int i = 0; i < 0x10; i++)
		GXSetTevDirect(i);

	GXSetNumChans(0);
	GXSetNumTexGens(1);

	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY, GX_FALSE, GX_DTTIDENTITY);

	GXSetNumTevStages(1);
	GXSetNumIndStages(0);

	GXSetTevSwapModeTable(GX_TEV_SWAP0, GX_CH_RED, GX_CH_GREEN, GX_CH_BLUE, GX_CH_ALPHA);

	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_C1, GX_CC_C0, GX_CC_TEXC, GX_CC_ZERO);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);

	GXSetZCompLoc(GX_FALSE);
	GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_SET);
	GXSetZMode(GX_TRUE, GX_LEQUAL, GX_FALSE);
	GXSetAlphaCompare(GX_GREATER, 0, GX_AOP_OR, GX_GREATER, 0);

	GXSetFog(GX_FOG_NONE, 0, 0, 0, 0, (GXColor){0,0,0,0});
	GXSetFogRangeAdj(GX_FALSE, 0, 0);

	GXSetCullMode(GX_CULL_NONE);

	GXSetDither(GX_TRUE);

	GXSetTevColor(GX_TEVREG0, (GXColor){255,255,255,255});
	GXSetTevColor(GX_TEVREG1, (GXColor){0,0,0,255});
}

void dWMMap_c::renderer_c::renderTileLayer(dKPLayer_s *layer, dKPLayer_s::sector_s *sectors) {
	//TileReport("Rendering layer %p\n", layer);

	// don't render it if we don't need to
	if (maxX < layer->left || minX > layer->right)
		return;
	if (maxY < layer->top || minY > layer->bottom)
		return;

	// set up
	loadCamera();

	loadTexture(layer->tileset);

	// figure out -what- to render
	BoundReport("Regular render area: %d,%d to %d,%d\n", minX, minY, maxX, maxY);
	BoundReport("Layer bounds: %d,%d to %d,%d\n", layer->left, layer->top, layer->right, layer->bottom);
	int toRenderMinX = max(minX, layer->left);
	int toRenderMinY = max(minY, layer->top);

	int toRenderMaxX = min(maxX, layer->right);
	int toRenderMaxY = min(maxY, layer->bottom);

	int sectorBaseX = layer->sectorLeft;
	int sectorBaseY = layer->sectorTop;
	int sectorIndexStride = (layer->sectorRight - layer->sectorLeft + 1);

	int sectorMinX = toRenderMinX / 16;
	int sectorMinY = toRenderMinY / 16;
	int sectorMaxX = toRenderMaxX / 16;
	int sectorMaxY = toRenderMaxY / 16;
	BoundReport("To render: %d,%d to %d,%d\n", toRenderMinX, toRenderMinY, toRenderMaxX, toRenderMaxY);
	BoundReport("sectors %d,%d to %d,%d\n", sectorMinX, sectorMinY, sectorMaxX, sectorMaxY);

	for (int sectorY = sectorMinY; sectorY <= sectorMaxY; sectorY++) {
		int baseIndex = (sectorY - sectorBaseY) * sectorIndexStride;

		int iMinY = (sectorY == sectorMinY) ? (toRenderMinY & 0xF) : 0;
		int iMaxY = (sectorY == sectorMaxY) ? (toRenderMaxY & 0xF) : 15;

		int worldSectorY = sectorY << 4;

		for (int sectorX = sectorMinX; sectorX <= sectorMaxX; sectorX++) {
			u16 index = layer->indices[baseIndex + sectorX - sectorBaseX];
			TileReport("Sector index @ %d,%d: %d\n", sectorX, sectorY, index);
			if (index == 0xFFFF)
				continue;

			dKPLayer_s::sector_s *sector = &sectors[index];

			int iMinX = (sectorX == sectorMinX) ? (toRenderMinX & 0xF) : 0;
			int iMaxX = (sectorX == sectorMaxX) ? (toRenderMaxX & 0xF) : 15;
			TileReport("Min/Max: X: %d,%d Y: %d,%d\n", iMinX, iMaxX, iMinY, iMaxY);

			int worldSectorX = sectorX << 4;

			for (int inY = iMinY; inY <= iMaxY; inY++) {
				for (int inX = iMinX; inX <= iMaxX; inX++) {
					u16 tileID = (*sector)[inY][inX];
					if (tileID == 0xFFFF)
						continue;

					s16 worldX = (worldSectorX | inX) * 24;
					s16 worldY = -((worldSectorY | inY) * 24);

					TileReport("Drawing tile %d at %d,%d\n", tileID, worldX, worldY);
					
					float tileX = (float)((tileID & 0x1F) * 28);
					float tileY = (float)(((tileID & 0x1E0) >> 5) * 28);

					float xMult = (1.0f / 896.0f);
					float yMult = (1.0f / 448.0f);

					float coordX1 = xMult * (tileX + 2.0f);
					float coordX2 = xMult * (tileX + 26.0f);
					float coordY1 = yMult * (tileY + 2.0f);
					float coordY2 = yMult * (tileY + 26.0f);


					GXBegin(GX_QUADS, GX_VTXFMT0, 4);
					GXPosition2s16(worldX + 24, worldY - 24);
					GXTexCoord2f32(coordX2, coordY2);
					GXPosition2s16(worldX + 24, worldY);
					GXTexCoord2f32(coordX2, coordY1);
					GXPosition2s16(worldX, worldY);
					GXTexCoord2f32(coordX1, coordY1);
					GXPosition2s16(worldX, worldY - 24);
					GXTexCoord2f32(coordX1, coordY2);
					GXEnd();
				}
			}

			TileReport("Sector complete\n");
		}
	}

	//TileReport("Layer complete\n");
}

void dWMMap_c::renderer_c::renderDoodadLayer(dKPLayer_s *layer) {
	for (int i = 0; i < layer->doodadCount; i++) {
		dKPDoodad_s *doodad = layer->doodads[i];
		DoodadReport("Doodad @ %f,%f sized %f,%f with angle %f\n", doodad->x, doodad->y, doodad->width, doodad->height, doodad->angle);

		// ANIMATE THE FUCKER
		float effectiveX = doodad->x, effectiveY = doodad->y;
		float effectiveWidth = doodad->width, effectiveHeight = doodad->height;
		float effectiveAngle = doodad->angle;
		int effectiveAlpha = layer->alpha;

		if (doodad->animationCount > 0) {
			for (int j = 0; j < doodad->animationCount; j++) {
				dKPDoodad_s::animation_s *anim = &doodad->animations[j];

				if (anim->delayOffset == 0) {
					u32 baseTick = anim->baseTick;
					if (baseTick == 0) {
						anim->baseTick = baseTick = GlobalTickCount;
					}

					u32 elapsed = GlobalTickCount - baseTick;
					if (anim->isReversed)
						elapsed = anim->frameCount - 1 - elapsed;
					u32 elapsedAdjusted = elapsed;

					if (elapsed >= anim->frameCount) {
						if (elapsed >= (anim->frameCount + anim->delay)) {

							// we've reached the end
							switch (anim->loop) {
								case dKPDoodad_s::animation_s::CONTIGUOUS:
									// Stop here
									elapsed = anim->frameCount - 1;
									break;

								case dKPDoodad_s::animation_s::LOOP:
									// Start over
									elapsed = 0;
									anim->baseTick = GlobalTickCount;
									break;

								case dKPDoodad_s::animation_s::REVERSE_LOOP:
									// Change direction
									anim->isReversed = !anim->isReversed;
									elapsed = (anim->isReversed) ? (anim->frameCount - 1) : 0;
									anim->baseTick = GlobalTickCount;
									break;
							}
							elapsedAdjusted = elapsed;
						} else {
							elapsedAdjusted = anim->frameCount;
						}
					}

					// now calculate the thing
					float progress = elapsedAdjusted / (float)anim->frameCount;
					// float progress = elapsed / (float)anim->frameCount;
					float value;

					switch (anim->curve) {
						case dKPDoodad_s::animation_s::LINEAR:
							value = progress;
							break;
						case dKPDoodad_s::animation_s::SIN:
							value = (sin(((progress * M_PI * 2)) - M_PI_2) + 1) / 2;
							break;
						case dKPDoodad_s::animation_s::COS:
							value = (cos(((progress * M_PI * 2)) - M_PI_2) + 1) / 2;
							break;
					}

					float delta = anim->end - anim->start;
					float frame;

					if (anim->isReversed)
						frame = anim->start + ceil(delta * value);
					else
						frame = anim->start + (delta * value);

					float scaleYMod;
					// and apply it!
					switch (anim->type) {
						case dKPDoodad_s::animation_s::X_POS:
							effectiveX += frame;
							break;
						case dKPDoodad_s::animation_s::Y_POS:
							effectiveY += frame;
							break;
						case dKPDoodad_s::animation_s::ANGLE:
							effectiveAngle += frame;
							break;
						case dKPDoodad_s::animation_s::X_SCALE:
							effectiveWidth = (effectiveWidth * frame / 100.0);
							break;
						case dKPDoodad_s::animation_s::Y_SCALE:
							effectiveHeight = (effectiveHeight * frame / 100.0);

							scaleYMod = doodad->height - effectiveHeight;
							effectiveY += scaleYMod;
							break;
						case dKPDoodad_s::animation_s::OPACITY:
							effectiveAlpha = (effectiveAlpha * (frame * 2.55f)) / 255;
							break;
					}
				}
				else { 
				anim->delayOffset -= 1;
				}
			}
		}

		float halfW = effectiveWidth * 0.5f, halfH = effectiveHeight * 0.5f;

		Mtx doodadMtx;
		MTXTransApply(renderMtx, doodadMtx, effectiveX + halfW, -effectiveY - halfH, 0);

		if (effectiveAngle != 0) {
			Mtx rotMtx;
			MTXRotDeg(rotMtx, 'z', -effectiveAngle);

			MTXConcat(doodadMtx, rotMtx, doodadMtx);
		}

		GXColor whichCol = (GXColor){255,255,255,u8(effectiveAlpha)};
		GXSetTevColor(GX_TEVREG0, whichCol);

		loadCamera(doodadMtx);
		loadTexture(doodad->texObj);

		GXBegin(GX_QUADS, GX_VTXFMT1, 4);
		GXPosition2f32(halfW, -halfH);
		GXTexCoord2u8(255, 255);
		GXPosition2f32(halfW, halfH);
		GXTexCoord2u8(255, 0);
		GXPosition2f32(-halfW, halfH);
		GXTexCoord2u8(0, 0);
		GXPosition2f32(-halfW, -halfH);
		GXTexCoord2u8(0, 255);
		GXEnd();
	}
}

void dWMMap_c::renderPathLayer(dKPLayer_s *layer) {

	for (int i = 0; i < layer->nodeCount; i++) {
		dKPNode_s *node = layer->nodes[i];

		if (node->type == dKPNode_s::LEVEL) {
			if (node->levelNumber[0] == 80)
				continue;

			short rz = 0x6000;
			short rx = 0x4000;
			short ry = 0x8000;

			node->extra->matrix.translation(node->x, -node->y + 4.0, 498.0);
			node->extra->matrix.applyRotationYXZ(&ry, &rx, &rz);
			node->extra->model.setDrawMatrix(node->extra->matrix);
			node->extra->model.setScale(0.8f, 0.8f, 0.8f);
			node->extra->model.calcWorld(false);

			node->extra->model.scheduleForDrawing();
		}
	}
}


void dWMMap_c::renderer_c::endRendering() {
}

void dWMMap_c::renderer_c::loadTexture(GXTexObj *obj) {
	if (currentTexture == obj)
		return;

	GXLoadTexObj(obj, GX_TEXMAP0);
	currentTexture = obj;
}

void dWMMap_c::renderer_c::loadCamera() {
	GXLoadPosMtxImm(renderMtx, GX_PNMTX0);
}

void dWMMap_c::renderer_c::loadCamera(Mtx m) {
	GXLoadPosMtxImm(m, GX_PNMTX0);
}



void dWMMap_c::doEffects() {
	int mapID = dScKoopatlas_c::instance->currentMapID;

	// Note: effect::spawn() takes name, unk, pos, rot, scale
	const S16Vec efRot = {0x1800, 0, 0};

#ifdef FALLING_LEAF
	if (mapID == 0) {
		// Torches
		static const VEC3 torchPos[6] = {
			{5122.0f, -5750.0f, 7000.0f},
			{4980.0f, -5700.0f, 7000.0f},
			{5035.0f, -5674.0f, 7000.0f},
			{5110.0f, -5674.0f, 7000.0f},
			{4980.0f, -5510.0f, 7000.0f},
			{5144.0f, -5474.0f, 7000.0f},
		};
		const VEC3 reallyBigScale = {1.6f, 1.6f, 1.6f};
		const VEC3 bigScale = {1.2f, 1.2f, 1.2f};
		const VEC3 smallScale = {0.75f, 0.75f, 0.75f};
		for (int i = 0; i < 6; i++) {
			const VEC3 *whichScale = &smallScale;
			if (i == 1)
				whichScale = &bigScale;
			else if (i == 3 || i == 5)
				whichScale = &reallyBigScale;
			effects[i].spawn("Wm_cs_torch", 0, &torchPos[i], &efRot, whichScale);
		}

		// Mountain Snow
		const VEC3 efPos = {5500.0f, -5050.0f, 7000.0f};
		effects[6].spawn("Wm_cs_snow_b", 0, &efPos, &efRot, 0);
	}
#else
	if (mapID == 1) {
		// Fullmap.
		// Torches
		static const VEC3 torchPos[6] = {
			{8402.0f, -5528.0f, 7000.0f}, // Big Tower
			{8444.0f, -5524.0f, 7000.0f}, // Tower
			{8358.0f, -5524.0f, 7000.0f}, // Tower
			{8420.0f, -5534.0f, 7000.0f}, // Tower
			{8380.0f, -5534.0f, 7000.0f}, // Tower
			{7804.0f, -5064.0f, 7000.0f}, // Castle
		};
		const VEC3 reallyBigScale = {1.6f, 1.6f, 1.6f};
		const VEC3 bigScale = {1.2f, 1.2f, 1.2f};
		const VEC3 smallScale = {0.25f, 0.25f, 0.25f};
		for (int i = 0; i < 6; i++) {
			const VEC3 *whichScale = &smallScale;
			if (i == 0)
				whichScale = &bigScale;
			else if (i == 5)
				whichScale = &reallyBigScale;
			effects[i].spawn("Wm_cs_torch", 0, &torchPos[i], &efRot, whichScale);
		}

		// Mountain Snow
		const VEC3 efPos = {6000.0f, -5250.0f, 7000.0f};
		effects[6].spawn("Wm_cs_snow_b", 0, &efPos, &efRot, 0);
	}
#endif

	if (mapID == 4) {
		// Freezeflame Volcano -- DONE
		const VEC3 efPos = {2200.0f, -2000.0f, 7000.0f};
		effects[0].spawn("Wm_cs_firespark", 0, &efPos, &efRot, 0);
	}

	if (mapID == 6) {
		// Koopa Planet -- DONE
		const VEC3 efPos = {2200.0f, -2000.0f, 7000.0f};
		effects[0].spawn("Wm_cs_firespark", 0, &efPos, &efRot, 0);
	}

	if (mapID == 7) {
		// Koopa Core -- DONE
		// Main area
		const VEC3 efPos = {2500.0f, -2900.0f, 7000.0f};
		effects[0].spawn("Wm_cs_firespark", 0, &efPos, &efRot, 0);
		// Castle area
		const VEC3 efPos2 = {4500.0f, -3800.0f, 7000.0f};
		effects[1].spawn("Wm_cs_firespark", 0, &efPos2, &efRot, 0);
		// Challenge House area
		const VEC3 efPos3 = {2500.0f, -5500.0f, 7000.0f};
		effects[2].spawn("Wm_cs_firespark", 0, &efPos2, &efRot, 0);
	}

	if (mapID == 3) {
		// Mountain Backside -- DONE
		VEC3 efPos = {3930.0f, -2700.0f, 7000.0f};
		effects[0].spawn("Wm_cs_snow_a", 0, &efPos, &efRot, 0);
		efPos.y -= 700.0f;
		effects[1].spawn("Wm_cs_snow_a", 0, &efPos, &efRot, 0);
	}

}


void dWMMap_c::spinLaunchStar() {
	nw4r::g3d::ResFile lsRes(getResource("StarRing", "g3d/StarRing.brres"));
	nw4r::g3d::ResAnmChr lsAnm = lsRes.GetResAnmChr("StarRing_shot");
	launchStarAnm.bind(&launchStarModel, lsAnm, false);
	launchStarModel.bindAnim(&launchStarAnm);
}


//
// processed\../src/koopatlas/mapdata.cpp
//

#include "koopatlas/mapdata.h"


// HELPER FUNCTIONS
dKPPath_s *dKPNode_s::getOppositeExitTo(dKPPath_s *path, bool mustBeAvailable) {
	for (int i = 0; i < 4; i++) {
		dKPPath_s *check = exits[i];

		if (check != 0 && check != path) {
			if (mustBeAvailable && !check->isAvailable)
				continue;

			return check;
		}
	}

	return 0;
}

int dKPNode_s::getExitCount(bool mustBeAvailable) {
	int ct = 0;

	for (int i = 0; i < 4; i++)
		if (exits[i] && (mustBeAvailable ? exits[i]->isAvailable : true))
			ct++;

	return ct;
}

bool dKPNode_s::isUnlocked() {
	for (int i = 0; i < 4; i++)
		if (exits[i]) 
			if (exits[i]->isAvailable)
				return true; 
	return false;
}

void dKPNode_s::setupNodeExtra() {
	int world = this->levelNumber[0];
	int level = this->levelNumber[1];

	SaveBlock *save = GetSaveFile()->GetBlock(-1);
	u32 conds = save->GetLevelCondition(world-1, level-1);

	bool isUnlocked = this->isUnlocked();
	bool exitComplete = false;
	bool secretComplete = false;

	if (conds & 0x10)
		exitComplete = true;
	if (conds & 0x20)
		secretComplete = true;

	const char *colour;

	//OSReport("Level %d-%d, isUnlocked: %d, exitComplete: %d", world, level, isUnlocked, exitComplete);

	// default: non-unlocked levels AND completed one-time levels
	colour = "g3d/black.brres";

	// one-time levels
	if ((level >= 30) && (level <= 37)) {
		if (isUnlocked && !exitComplete)
			colour = "g3d/red.brres";
	}
	// the shop
	else if (level == 99)
		colour = "g3d/shop.brres";

	else if (isUnlocked) {
		if (hasSecret) {
			if (exitComplete && secretComplete)
				colour = "g3d/blue.brres";
			else if (exitComplete || secretComplete)
				colour = "g3d/purple.brres";
			else
				colour = "g3d/red.brres";
		} else {
			if (exitComplete)
				colour = "g3d/blue.brres";
			else
				colour = "g3d/red.brres";
		}
	}

	// model time
	this->extra->mallocator.link(-1, GameHeaps[0], 0, 0x20);

	nw4r::g3d::ResFile rg(getResource("cobCourse", colour));
	this->extra->model.setup(rg.GetResMdl("cobCourse"), &this->extra->mallocator, 0x224, 1, 0);
	this->extra->matrix.identity();
	SetupTextures_MapObj(&this->extra->model, 0);

	this->extra->mallocator.unlink();
}

void dKPNode_s::setLayerAlpha(u8 alpha) {
	if (tileLayer)
		tileLayer->alpha = alpha;
	if (doodadLayer)
		doodadLayer->alpha = alpha;
}

void dKPPath_s::setLayerAlpha(u8 alpha) {
	if (tileLayer)
		tileLayer->alpha = alpha;
	if (doodadLayer)
		doodadLayer->alpha = alpha;
}

int dKPLayer_s::findNodeID(dKPNode_s *node) {
	for (int i = 0; i < nodeCount; i++)
		if (nodes[i] == node)
			return i;

	return -1;
}

dKPMapData_c::dKPMapData_c() {
	data = 0;
	fixedUp = false;
	levelNodeExtraArray = 0;
	tilesetLoaders = 0;
	tilesetsLoaded = false;
}

dKPMapData_c::~dKPMapData_c() {
	if (levelNodeExtraArray)
		delete[] levelNodeExtraArray;

	unloadTilesets();

	m_fileLoader.unload();
}

bool dKPMapData_c::load(const char *filename) {
	data = (dKPMapFile_s*)m_fileLoader.load(filename, 0, mHeap::archiveHeap);

	if (data == 0)
		return false;

	if (!fixedUp)
		fixup();

	bool didLoadTilesets = loadTilesets();
	bool didLoadBG = (bgLoader.load(data->backgroundName) != 0);

	return didLoadTilesets && didLoadBG;
}

bool dKPMapData_c::loadTilesets() {
	if (tilesetsLoaded)
		return true;

	if (tilesetLoaders == 0)
		tilesetLoaders = new dDvdLoader_c[data->tilesetCount];

	bool result = true;

	for (int i = 0; i < data->tilesetCount; i++) {
		char *filename = ((char*)data) + (data->tilesets[i].val[3] - 0x10000000);
		void *heap = (i < 9) ? mHeap::gameHeaps[2] : mHeap::archiveHeap;
		result &= (tilesetLoaders[i].load(filename, 0, heap) != 0);
	}

	if (result) {
		tilesetsLoaded = true;

		for (int i = 0; i < data->tilesetCount; i++) {
			data->tilesets[i].val[3] = (((u32)tilesetLoaders[i].buffer & ~0xC0000000) >> 5);
		}
	}

	return tilesetsLoaded;
}

void dKPMapData_c::unloadTilesets() {
	if (tilesetLoaders != 0) {
		for (int i = 0; i < data->tilesetCount; i++) {
			tilesetLoaders[i].unload();
		}

		delete[] tilesetLoaders;
	}

	bgLoader.unload();
}

void dKPMapData_c::fixup() {
	OSReport("Setting up Nodes");

	fixedUp = true;

	fixRef(data->layers);
	fixRef(data->tilesets);
	fixRef(data->unlockData);
	fixRef(data->sectors);
	fixRef(data->backgroundName);
	if (data->version >= 2) {
		fixRef(data->worlds);
		for (int i = 0; i < data->worldCount; i++) {
			fixRef(data->worlds[i].name);
		}
	}

	for (int iLayer = 0; iLayer < data->layerCount; iLayer++) {
		dKPLayer_s *layer = fixRef(data->layers[iLayer]);

		switch (layer->type) {
			case dKPLayer_s::OBJECTS:
				fixRef(layer->tileset);
				break;

			case dKPLayer_s::DOODADS:
				for (int iDood = 0; iDood < layer->doodadCount; iDood++) {
					dKPDoodad_s *doodad = fixRef(layer->doodads[iDood]);

					fixRef(doodad->texObj);
					fixTexObjSafe(doodad->texObj);
				}
				break;
				
			case dKPLayer_s::PATHS:
				pathLayer = layer;

				fixRef(layer->paths);
				fixRef(layer->nodes);

				for (int iPath = 0; iPath < layer->pathCount; iPath++) {
					dKPPath_s *path = fixRef(layer->paths[iPath]);

					fixRef(path->start);
					fixRef(path->end);

					fixRef(path->tileLayer);
					fixRef(path->doodadLayer);
				}

				for (int iNode = 0; iNode < layer->nodeCount; iNode++) {
					dKPNode_s *node = fixRef(layer->nodes[iNode]);

					for (int i = 0; i < 4; i++)
						fixRef(node->exits[i]);

					fixRef(node->tileLayer);
					fixRef(node->doodadLayer);

					if (node->type == dKPNode_s::CHANGE) {
						fixRef(node->destMap);
						OSReport("Node: %x, %s", node->destMap, node->destMap); }

				}
				break;
		}
	}

	
	// before we finish here, create the Node Extra classes

	// first off, count how many we need...
	int count = 0;
	for (int nodeIdx = 0; nodeIdx < pathLayer->nodeCount; nodeIdx++) {
		if (pathLayer->nodes[nodeIdx]->type == dKPNode_s::LEVEL)
			count++;
	}

	levelNodeExtraArray = new dKPNodeExtra_c[count];

	int extraIdx = 0;

	for (int nodeIdx = 0; nodeIdx < pathLayer->nodeCount; nodeIdx++) {
		if (pathLayer->nodes[nodeIdx]->type == dKPNode_s::LEVEL) {
			pathLayer->nodes[nodeIdx]->extra = &levelNodeExtraArray[extraIdx];
			extraIdx++;
		}
	}
}


const dKPWorldDef_s *dKPMapData_c::findWorldDef(int id) const {
	for (int i = 0; i < data->worldCount; i++) {
		if (data->worlds[i].key == id)
			return &data->worlds[i];
	}

	return 0;
}


/******************************************************************************
 * Generic Layer
 ******************************************************************************/


/******************************************************************************
 * Tile Layer
 ******************************************************************************/


/******************************************************************************
 * Doodad Layer
 ******************************************************************************/


/******************************************************************************
 * Path Layer
 ******************************************************************************/




//
// processed\../src/koopatlas/pathmanager.cpp
//

#include "koopatlas/pathmanager.h"
#include "koopatlas/core.h"
#include "koopatlas/hud.h"
#include "koopatlas/player.h"
#include "koopatlas/map.h"
#include <sfx.h>
#include <stage.h>

extern "C" void PlaySoundWithFunctionB4(void *spc, nw4r::snd::SoundHandle *handle, int id, int unk);

u8 MaybeFinishingLevel[2] = {0xFF,0xFF};
u8 LastLevelPlayed[2] = {0xFF,0xFF};
bool CanFinishCoins = false;
bool CanFinishExits = false;
bool CanFinishWorld = false;
bool CanFinishAlmostAllCoins = false;
bool CanFinishAllCoins = false;
bool CanFinishAllExits = false;
bool CanFinishEverything = false;
void ResetAllCompletionCandidates() {
	// This is called by File Select, btw
	MaybeFinishingLevel[0] = 0xFF;
	LastLevelPlayed[0] |= 0x80;
	CanFinishCoins = false;
	CanFinishExits = false;
	CanFinishWorld = false;
	CanFinishAlmostAllCoins = false;
	CanFinishAllCoins = false;
	CanFinishAllExits = false;
	CanFinishEverything = false;
}

int getPressedDir(int buttons) {
	if (buttons & WPAD_LEFT) return 0;
	else if (buttons & WPAD_RIGHT) return 1;
	else if (buttons & WPAD_UP) return 2;
	else if (buttons & WPAD_DOWN) return 3;
	return -1;
}

void dWMPathManager_c::setup() {
	dScKoopatlas_c *wm = dScKoopatlas_c::instance;

	isMoving = false;
	isJumping = false;
	scaleAnimProgress = -1;
	timer = 0.0;
	currentPath = 0;
	reverseThroughPath = false;

	shouldRequestSave = ((wm->settings & 0x80000) != 0);
	checkedForMoveAfterEndLevel = ((wm->settings & 0x40000) != 0);
	afterFortressMode = ((wm->settings & 0x20000) != 0);

	pathLayer = wm->mapData.pathLayer;

	SpammyReport("setting up PathManager\n");
	SaveBlock *save = GetSaveFile()->GetBlock(-1);

	mustComplainToMapCreator = false;

	SpammyReport("Unlocking paths\n");
	isEnteringLevel = false;
	levelStartWait = -1;
	unlockPaths();

	waitAfterInitialPlayerAnim = -1;
	if (LastPowerupStoreType == LOSE_LEVEL) {
		mustPlayAfterDeathAnim = true;
		daWMPlayer_c::instance->visible = false;
		LastPowerupStoreType = BEAT_LEVEL;
	} else if (LastPowerupStoreType == BEAT_LEVEL && LastLevelPlayed[0] < 0x80 && !wm->isAfter8Castle) {
		mustPlayAfterWinAnim = true;
		daWMPlayer_c::instance->visible = false;
	}

	SpammyReport("done\n");

	// Figure out what path node to start at
	if (wm->settings & 0x10000000) {
		// Start off from a "Change"
		u8 changeID = (wm->settings >> 20) & 0xFF;
		SpammyReport("entering at Change ID %d\n", changeID);
		SpammyReport("Path layer: %p\n", pathLayer);
		SpammyReport("Node count: %d\n", pathLayer->nodeCount);

		bool found = false;

		for (int i = 0; i < pathLayer->nodeCount; i++) {
			dKPNode_s *node = pathLayer->nodes[i];
			SpammyReport("Checking node: %p\n", node);

			if (node->type == dKPNode_s::CHANGE && node->thisID == changeID) {
				found = true;
				currentNode = node;

				SpammyReport("Found CHANGE node: %d %p\n", changeID, node);

				// figure out where we should move to
				dKPPath_s *exitTo = 0;

				for (int i = 0; i < 4; i++) {
					dKPPath_s *candidateExit = node->exits[i];
					//OSReport("Candidate exit: %p\n", candidateExit);
					if (!candidateExit)
						continue;

					// find out if this path is a candidate
					dKPNode_s *srcNode = node;
					dKPPath_s *path = candidateExit;

					while (true) {
						dKPNode_s *destNode = (path->start == srcNode) ? path->end : path->start;
						//OSReport("Path: %p nodes %p to %p\n", path, srcNode, destNode);
						int ct = destNode->getAvailableExitCount();
						//OSReport("Dest Node available exits: %d; type: %d\n", ct, destNode->type);
						if (destNode == node || ct > 2 || destNode->type == dKPNode_s::LEVEL || destNode->type == dKPNode_s::CHANGE) {
							exitTo = candidateExit;
							//OSReport("Accepting this node\n");
							break;
						}

						if (ct == 1)
							break;

						// where to next?
						path = destNode->getOppositeAvailableExitTo(path);
						srcNode = destNode;
					}

					if (exitTo)
						break;
				}

				if (!exitTo)
					exitTo = node->getAnyExit();
				startMovementTo(exitTo);
				break;
			}
		}

		if (!found) {
			currentNode = pathLayer->nodes[0];
			mustComplainToMapCreator = true;
		}

		waitAtStart = 1;
	} else {
		if (!countdownToFadeIn)
			waitAtStart = 50;
		else
			waitAtStart = 1;

		if (wm->isFirstPlay)
			waitAtStart = 280;

		SpammyReport("saved path node: %d\n", save->current_path_node);
		if (save->current_path_node >= pathLayer->nodeCount) {
			SpammyReport("out of bounds (%d), using node 0\n", pathLayer->nodeCount);
			currentNode = pathLayer->nodes[0];
		} else {
			int butts = Remocon_GetButtons(GetActiveRemocon());
			if ((butts & WPAD_MINUS) && (butts & WPAD_PLUS) && (butts & WPAD_ONE))
				currentNode = pathLayer->nodes[0];
			else
				currentNode = pathLayer->nodes[save->current_path_node];
			SpammyReport("OK %p\n", currentNode);
		}

		int findW = -1, findL = -1;
		bool storeIt = true;

		if (wm->isAfterKamekCutscene) {
			findW = 8;
			findL = 1;
		} else if (wm->isAfter8Castle) {
			findW = 8;
			findL = 5;
		} else if (wm->isEndingScene) {
			findW = 80;
			findL = 80;
			storeIt = false;
		}
		if (findW > -1) {
			// look for the 8-1 node
			for (int i = 0; i < pathLayer->nodeCount; i++) {
				dKPNode_s *node = pathLayer->nodes[i];
				if (node->type == dKPNode_s::LEVEL && node->levelNumber[0] == findW && node->levelNumber[1] == findL) {
					currentNode = node;
					if (storeIt)
						save->current_path_node = i;
					break;
				}
			}
		}
	}

	for (int i = 0; i < pathLayer->nodeCount; i++)
		if (pathLayer->nodes[i]->type == dKPNode_s::LEVEL)
			pathLayer->nodes[i]->setupNodeExtra();

	// did we just beat a level?
	if (MaybeFinishingLevel[0] != 0xFF) {
		if (save->CheckLevelCondition(MaybeFinishingLevel[0], MaybeFinishingLevel[1], COND_NORMAL)) {
			shouldRequestSave = true;
		}
	}

	// have we got any completions?
	if (LastLevelPlayed[0] < 0x80) {
		u32 conds = save->GetLevelCondition(LastLevelPlayed[0], LastLevelPlayed[1]);
		dLevelInfo_c::entry_s *whatEntry =
			dLevelInfo_c::s_info.searchBySlot(LastLevelPlayed[0], LastLevelPlayed[1]);

		// how many exits?
		int exits = 0, maxExits = 0;
		if (whatEntry->flags & 0x10) {
			maxExits++;
			if (conds & COND_NORMAL)
				exits++;
		}
		if (whatEntry->flags & 0x20) {
			maxExits++;
			if (conds & COND_SECRET)
				exits++;
		}

		completionMessageWorldNum = whatEntry->displayWorld;

		// now do all the message checks
		int flag = 0, totalFlag = 0;
		if (CanFinishCoins) {
			totalFlag |= 1;
			if ((conds & COND_COIN_ALL) == COND_COIN_ALL) {
				flag |= 1;
				completionMessageType = CMP_MSG_COINS;
			}
		}
		if (CanFinishExits) {
			totalFlag |= 2;
			if (exits == maxExits) {
				flag |= 2;
				completionMessageType = CMP_MSG_EXITS;
			}
		}
		if (CanFinishWorld && flag == totalFlag) {
			shouldRequestSave = true;
			completionMessageType = CMP_MSG_WORLD; 
		}

		if (CanFinishAlmostAllCoins) {
			if ((conds & COND_COIN_ALL) == COND_COIN_ALL)
				completionMessageType = CMP_MSG_GLOBAL_COINS_EXC_W9;
		}

		int gFlag = 0, gTotalFlag = 0;
		if (CanFinishAllCoins) {
			gTotalFlag |= 1;
			if ((conds & COND_COIN_ALL) == COND_COIN_ALL) {
				gFlag |= 1;
				completionMessageType = CMP_MSG_GLOBAL_COINS;
			}
		}
		if (CanFinishAllExits) {
			gTotalFlag |= 2;
			if (exits == maxExits) {
				gFlag |= 2;
				completionMessageType = CMP_MSG_GLOBAL_EXITS;
			}
		}
		if (CanFinishEverything && gFlag == gTotalFlag) {
			save->titleScreenWorld = 3;
			save->titleScreenLevel = 10;

			shouldRequestSave = true;
			completionMessageType = CMP_MSG_EVERYTHING;
		}
	}

	ResetAllCompletionCandidates();

	if (wm->isAfterKamekCutscene || wm->isAfter8Castle || wm->isEndingScene)
		copyWorldDefToSave(wm->mapData.findWorldDef(1));

	finalisePathUnlocks();
}

static u8 *PathAvailabilityData = 0;
static u8 *NodeAvailabilityData = 0;

void ClearOldPathAvailabilityData() {
	// This is called by File Select
	if (PathAvailabilityData) {
		delete[] PathAvailabilityData;
		PathAvailabilityData = 0;
	}
	if (NodeAvailabilityData) {
		delete[] NodeAvailabilityData;
		NodeAvailabilityData = 0;
	}
}

dWMPathManager_c::~dWMPathManager_c() {
	bool entering8_25 = (MaybeFinishingLevel[0] == 7) && (MaybeFinishingLevel[1] == 24);
	if (!isEnteringLevel && !entering8_25) {
		ClearOldPathAvailabilityData();
	}

	if (isEnteringLevel) {
		ResetAllCompletionCandidates();

		SaveBlock *save = GetSaveFile()->GetBlock(-1);
		if ((enteredLevel->displayLevel >= 21 && enteredLevel->displayLevel <= 27 && enteredLevel->displayLevel != 26)
				|| (enteredLevel->displayLevel >= 29 && enteredLevel->displayLevel <= 42)) {
			if (!save->CheckLevelCondition(enteredLevel->worldSlot, enteredLevel->levelSlot, COND_NORMAL)) {
				MaybeFinishingLevel[0] = enteredLevel->worldSlot;
				MaybeFinishingLevel[1] = enteredLevel->levelSlot;
			}
		}

		LastLevelPlayed[0] = enteredLevel->worldSlot;
		LastLevelPlayed[1] = enteredLevel->levelSlot;

		// Now, a fuckton of checks for the various possible things we can finish!
		dLevelInfo_c *li = &dLevelInfo_c::s_info;
		u32 theseConds = save->GetLevelCondition(enteredLevel->worldSlot, enteredLevel->levelSlot);

		int coinCount = 0, exitCount = 0;
		int globalCoinCount = 0, globalCoinCountExcW9 = 0, globalExitCount = 0;

		int totalCoinCount = 0, totalExitCount = 0;
		int totalGlobalCoinCount = 0, totalGlobalCoinCountExcW9 = 0, totalGlobalExitCount = 0;

		for (int sIdx = 0; sIdx < li->sectionCount(); sIdx++) {
			dLevelInfo_c::section_s *sect = li->getSectionByIndex(sIdx);

			for (int lIdx = 0; lIdx < sect->levelCount; lIdx++) {
				dLevelInfo_c::entry_s *entry = &sect->levels[lIdx];
				u32 entryConds = save->GetLevelCondition(entry->worldSlot, entry->levelSlot);

				// Only track actual levels
				if (!(entry->flags & 2))
					continue;

				// Counts for this world
				if (entry->displayWorld == enteredLevel->displayWorld) {
					totalCoinCount++;
					if ((entryConds & COND_COIN_ALL) == COND_COIN_ALL)
						coinCount++;

					// Normal exit
					if (entry->flags & 0x10) {
						totalExitCount++;
						if (entryConds & COND_NORMAL)
							exitCount++;
					}

					// Secret exit
					if (entry->flags & 0x20) {
						totalExitCount++;
						if (entryConds & COND_SECRET)
							exitCount++;
					}
				}

				// Counts for everywhere
				totalGlobalCoinCount++;
				if ((entryConds & COND_COIN_ALL) == COND_COIN_ALL)
					globalCoinCount++;

				if (entry->displayWorld != 9) {
					totalGlobalCoinCountExcW9++;
					if ((entryConds & COND_COIN_ALL) == COND_COIN_ALL)
						globalCoinCountExcW9++;
				}

				// Normal exit
				if (entry->flags & 0x10) {
					totalGlobalExitCount++;
					if (entryConds & COND_NORMAL)
						globalExitCount++;
				}

				// Secret exit
				if (entry->flags & 0x20) {
					totalGlobalExitCount++;
					if (entryConds & COND_SECRET)
						globalExitCount++;
				}
			}
		}

		// I'm using gotos. SUE ME.
		// Anyhow, don't consider non-levels for this.
		if (!(enteredLevel->flags & 2))
			goto cannotFinishAnything;

		// So.. are we candidates for any of these?
		int everythingFlag = 0, gEverythingFlag = 0;
		if (coinCount == totalCoinCount)
			everythingFlag |= 1;
		if (exitCount == totalExitCount)
			everythingFlag |= 2;
		if (globalCoinCount == totalGlobalCoinCount)
			gEverythingFlag |= 1;
		if (globalExitCount == totalGlobalExitCount)
			gEverythingFlag |= 2;

		// Check if we could obtain every star coin
		if ((theseConds & COND_COIN_ALL) != COND_COIN_ALL) {
			if ((coinCount + 1) == totalCoinCount) {
				CanFinishCoins = true;
				everythingFlag |= 1;
			}
			if ((globalCoinCount + 1) == totalGlobalCoinCount) {
				CanFinishAllCoins = true;
				gEverythingFlag |= 1;
			}
			if ((globalCoinCountExcW9 + 1) == totalGlobalCoinCountExcW9)
				CanFinishAlmostAllCoins = true;
		}

		// Check if we could obtain every exit
		int elExits = 0, elTotalExits = 0;
		if (enteredLevel->flags & 0x10) {
			elTotalExits++;
			if (theseConds & COND_NORMAL)
				elExits++;
		}
		if (enteredLevel->flags & 0x20) {
			elTotalExits++;
			if (theseConds & COND_SECRET)
				elExits++;
		}

		if ((elExits + 1) == elTotalExits) {
			if ((exitCount + 1) == totalExitCount) {
				CanFinishExits = true;
				everythingFlag |= 2;
			}
			if ((globalExitCount + 1) == totalGlobalExitCount) {
				CanFinishAllExits = true;
				gEverythingFlag |= 2;
			}
		}

		// And could we obtain EVERYTHING?
		if ((CanFinishCoins || CanFinishExits) && everythingFlag == 3)
			CanFinishWorld = true;
		if ((CanFinishAllCoins || CanFinishAllExits) && gEverythingFlag == 3)
			CanFinishEverything = true;
	}

cannotFinishAnything:
	if (penguinSlideSound.Exists())
		penguinSlideSound.Stop(5);
}

void dWMPathManager_c::unlockPaths() {
	u8 *oldPathAvData = PathAvailabilityData;
	PathAvailabilityData = new u8[pathLayer->pathCount];

	u8 *oldNodeAvData = NodeAvailabilityData;
	NodeAvailabilityData = new u8[pathLayer->nodeCount];

	SpammyReport("Unlocking paths\n");

	// unlock all needed paths
	for (int i = 0; i < pathLayer->pathCount; i++) {
		dKPPath_s *path = pathLayer->paths[i];

		PathAvailabilityData[i] = path->isAvailable;

		//SpammyReport("Path %d: %d\n", i, path->isAvailable);
		// if this path is not "always available", then nuke its alpha
		path->setLayerAlpha((path->isAvailable == dKPPath_s::ALWAYS_AVAILABLE) ? 255 : 0);
	}

	SaveBlock *save = GetSaveFile()->GetBlock(-1);

	cachedTotalStarCoinCount = getStarCoinCount();
	cachedUnspentStarCoinCount = cachedTotalStarCoinCount - save->spentStarCoins;

	u8 *in = (u8*)dScKoopatlas_c::instance->mapData.data->unlockData;
	SpammyReport("UNLOCKING PATHS: Unlock data @ %p\n", in);

	int cmdID = 0;

	while (*in != 0) {
		UnlockCmdReport("[%p] Cmd %d: Evaluating condition\n", in, cmdID);
		// begin processing a block
		bool value = evaluateUnlockCondition(in, save, 0);
		UnlockCmdReport("[%p] Cmd %d: Condition evaluated, result: %d\n", in, cmdID, value);
		//UnlockCmdReport("Unlock condition: %d\n", value);

		// get what it's supposed to affect
		// for now we'll assume that it affects one or more paths
		u8 affectedCount = *(in++);
		UnlockCmdReport("[%p] Cmd %d: Affects %d path(s)\n", in, cmdID, affectedCount);

		for (int i = 0; i < affectedCount; i++) {
			u8 one = *(in++);
			u8 two = *(in++);
			u16 pathID = (one << 8) | two;
			UnlockCmdReport("[%p] Cmd %d: Affected %d: PathID: %d\n", in, cmdID, i, pathID);

			dKPPath_s *path = pathLayer->paths[pathID];
			UnlockCmdReport("[%p] Cmd %d: Affected %d: Path: %p\n", in, cmdID, i, path);
			path->isAvailable = value ? dKPPath_s::AVAILABLE : dKPPath_s::NOT_AVAILABLE;
			UnlockCmdReport("[%p] Cmd %d: Affected %d: IsAvailable written\n", in, cmdID, i);
			PathAvailabilityData[pathID] = value ? dKPPath_s::AVAILABLE : dKPPath_s::NOT_AVAILABLE;
			UnlockCmdReport("[%p] Cmd %d: Affected %d: AvailabilityData written\n", in, cmdID, i);
			// NEWLY_AVAILABLE is set later, when that stuff is figured out

			path->setLayerAlpha(value ? 255 : 0);
			UnlockCmdReport("[%p] Cmd %d: Affected %d: Layer alpha applied\n", in, cmdID, i);
		}

		UnlockCmdReport("[%p] Cmd %d: %d affected path(s) processed\n", in, cmdID, affectedCount);

		cmdID++;
	}

	SpammyReport("UNLOCKING PATHS: All complete @ %p\n", in);

	for (int i = 0; i < pathLayer->nodeCount; i++) {
		dKPNode_s *node = pathLayer->nodes[i];
		NodeAvailabilityData[i] = node->isUnlocked();

		if (node->type == node->LEVEL && node->isUnlocked() && node->levelNumber[1] != 99) {
			save->completions[node->levelNumber[0]-1][node->levelNumber[1]-1] |= COND_UNLOCKED;
		}
	}

	// did anything become newly available?!
	newlyAvailablePaths = 0;
	newlyAvailableNodes = 0;

	dScKoopatlas_c *wm = dScKoopatlas_c::instance;
	bool forceFlag = (wm->isAfter8Castle || wm->isAfterKamekCutscene);

	if (!wm->isEndingScene && (oldPathAvData || forceFlag)) {
		for (int i = 0; i < pathLayer->pathCount; i++) {
			if ((PathAvailabilityData[i] > 0) && (forceFlag || oldPathAvData[i] == 0)) {
				if (forceFlag && PathAvailabilityData[i] == dKPPath_s::ALWAYS_AVAILABLE)
					continue;
	
				dKPPath_s *path = pathLayer->paths[i];
				path->isAvailable = dKPPath_s::NEWLY_AVAILABLE;
				newlyAvailablePaths++;

				// set this path's alpha to 0, we'll fade it in later
				path->setLayerAlpha(0);
			}
		}

		// check nodes too
		for (int i = 0; i < pathLayer->nodeCount; i++) {
			if ((NodeAvailabilityData[i] > 0) && (forceFlag || oldNodeAvData[i] == 0)) {
				dKPNode_s *node = pathLayer->nodes[i];
				node->isNew = true;
				newlyAvailableNodes++;
			}
		}
	}

	if (oldPathAvData) {
		delete[] oldPathAvData;
		delete[] oldNodeAvData;
	}

	if (wm->isEndingScene) {
		dKPNode_s *yoshiHouse = 0;
		for (int i = 0; i < pathLayer->nodeCount; i++) {
			dKPNode_s *node = pathLayer->nodes[i];

			if (node->type != dKPNode_s::LEVEL)
				continue;
			if (node->levelNumber[0] != 1)
				continue;
			if (node->levelNumber[1] != 41)
				continue;

			yoshiHouse = node;
			break;
		}

		if (yoshiHouse) {
			dKPNode_s *currentNode = yoshiHouse;
			dKPPath_s *nextPath = yoshiHouse->rightExit;

			while (true) {
				if (nextPath->isAvailable == dKPPath_s::AVAILABLE) {
					nextPath->isAvailable = dKPPath_s::NEWLY_AVAILABLE;
					newlyAvailablePaths++;
					nextPath->setLayerAlpha(0);
				}

				dKPNode_s *nextNode = nextPath->getOtherNodeTo(currentNode);
				if (!nextNode)
					break;

				if (nextNode->isUnlocked()) {
					nextNode->isNew = true;
					newlyAvailableNodes++;
				}

				currentNode = nextNode;
				nextPath = nextNode->getOppositeExitTo(nextPath);

				if (!nextPath)
					break;
			}
		}
	}

	// now set all node alphas
	for (int i = 0; i < pathLayer->nodeCount; i++) {
		dKPNode_s *node = pathLayer->nodes[i];

		node->setLayerAlpha((node->isUnlocked() & !node->isNew) ? 255 : 0);
	}
}

void dWMPathManager_c::finalisePathUnlocks() {
	// if anything was new, set it as such
	if (newlyAvailablePaths || newlyAvailableNodes) {
		countdownToFadeIn = 30;
		findCameraBoundsForUnlockedPaths();
	}
	unlockingAlpha = -1;
}

bool dWMPathManager_c::evaluateUnlockCondition(u8 *&in, SaveBlock *save, int stack) {
	UnlockCmdReport("[%p] CondStk:%d begin\n", in, stack);
	u8 controlByte = *(in++);

	u8 conditionType = (controlByte >> 6);
	UnlockCmdReport("[%p] CondStk:%d control byte: %d; condition type: %d\n", in, stack, controlByte, conditionType);

	if (conditionType == 0) {
		u8 subConditionType = controlByte & 0x3F;
		switch (subConditionType) {
			case 0: case 1: case 2: case 3:
				u8 one = *(in++);
				u8 two = *(in++);

				int compareOne = (one & 0x80) ? cachedUnspentStarCoinCount : cachedTotalStarCoinCount;
				int compareTwo = ((one & 0x7F) << 8) | two;

				switch (subConditionType) {
					case 0:
						return compareOne == compareTwo;
					case 1:
						return compareOne != compareTwo;
					case 2:
						return compareOne < compareTwo;
					case 3:
						return compareOne > compareTwo;
				}

			case 15:
				UnlockCmdReport("[%p] CondStk:%d end, returning CONSTANT 1\n", in, stack);
				return true;
			default:
				UnlockCmdReport("[%p] CondStk:%d unknown subCondition %d, returning 0\n", in, stack, subConditionType);
				return false;
		}
	}

	if (conditionType == 1) {
		// Simple level

		bool isSecret = (controlByte & 0x10);
		u8 worldNumber = controlByte & 0xF;
		u8 levelNumber = *(in++);
		UnlockCmdReport("[%p] CondStk:%d level, w:%d l:%d secret:%d\n", in, stack, worldNumber, levelNumber, isSecret);

		u32 conds = save->GetLevelCondition(worldNumber, levelNumber);
		UnlockCmdReport("[%p] CondStk:%d returning for level conditions: %d / %x\n", in, stack, conds, conds);

		if (isSecret)
			return (conds & COND_SECRET) != 0;
		else
			return (conds & COND_NORMAL) != 0;
	}

	// Type: 2 = AND, 3 = OR
	bool isAnd = (conditionType == 2);
	bool isOr = (conditionType == 3);

	bool value = isOr ? false : true;

	u8 termCount = (controlByte & 0x3F) + 1;
	UnlockCmdReport("[%p] CondStk:%d and:%d or:%d startValue:%d termCount:%d\n", in, stack, isAnd, isOr, value, termCount);

	for (int i = 0; i < termCount; i++) {
		bool what = evaluateUnlockCondition(in, save, stack+1);

		if (isOr)
			value |= what;
		else
			value &= what;
	}

	UnlockCmdReport("[%p] CondStk:%d end, returning %d\n", in, stack, value);
	return value;
}


bool dWMPathManager_c::doingThings() {
	if (isEnteringLevel || (waitAfterUnlock > 0) || (completionAnimDelay > 0) ||
			(waitAtStart > 0) || (waitAfterInitialPlayerAnim > 0) ||
			panningCameraToPaths || panningCameraFromPaths ||
			(waitBeforePanBack > 0) || !initialLoading ||
			(countdownToFadeIn > 0) || (unlockingAlpha != -1) ||
			dScKoopatlas_c::instance->isEndingScene)
		return true;

	if (isMoving)
		return true;

	return false;
}

void dWMPathManager_c::execute() {
	dScKoopatlas_c *wm = dScKoopatlas_c::instance;

	if (isEnteringLevel) {
		if (levelStartWait > 0) {
			levelStartWait--;
			if (levelStartWait == 0) {
				dScKoopatlas_c::instance->startLevel(enteredLevel);
			}
		}
		return;
	}

	if (waitAtStart > 0) {
		waitAtStart--;
		if (waitAtStart == 0) {
			if (mustPlayAfterDeathAnim) {
				daWMPlayer_c::instance->visible = true;
				daWMPlayer_c::instance->startAnimation(ending_wait, 1.0f, 0.0f, 0.0f);
				waitAfterInitialPlayerAnim = 60;

				nw4r::snd::SoundHandle something;
				PlaySoundWithFunctionB4(SoundRelatedClass, &something, SE_VOC_MA_CS_COURSE_MISS, 1);
			} else if (mustPlayAfterWinAnim) {
				daWMPlayer_c::instance->visible = true;
				if (dScKoopatlas_c::instance->isAfter8Castle) {
					waitAfterInitialPlayerAnim = 1;
				} else {
					daWMPlayer_c::instance->startAnimation(dm_surp_wait, 1.0f, 0.0f, 0.0f);
					waitAfterInitialPlayerAnim = 38;

					nw4r::snd::SoundHandle something;
					if (!wm->isEndingScene)
						PlaySoundWithFunctionB4(SoundRelatedClass, &something, SE_VOC_MA_CS_JUMP, 1);
				}
			}
		}
		return;
	}

	if (waitAfterInitialPlayerAnim > 0) {
		waitAfterInitialPlayerAnim--;
		if (waitAfterInitialPlayerAnim == 0)
			daWMPlayer_c::instance->startAnimation(wait_select, 1.0f, 0.0f, 0.0f);
		if (mustPlayAfterWinAnim && (waitAfterInitialPlayerAnim == 9) && !wm->isEndingScene) {
			nw4r::snd::SoundHandle something;
			PlaySoundWithFunctionB4(SoundRelatedClass, &something, SE_PLY_LAND_ROCK, 1);
		}
		return;
	}


	// handle path fading
	if (countdownToFadeIn > 0) {
		countdownToFadeIn--;
		if (countdownToFadeIn <= 0) {
			if (camBoundsValid) {
				dWorldCamera_c::instance->currentX = currentNode->x;
				dWorldCamera_c::instance->currentY = -currentNode->y;
				dWorldCamera_c::instance->panToBounds(camMinX, camMinY, camMaxX, camMaxY);

				panningCameraToPaths = true;
			} else
				unlockingAlpha = 0;

			nw4r::snd::SoundHandle something;
			PlaySoundWithFunctionB4(SoundRelatedClass, &something, SE_OBJ_GEN_LOAD, 1);
		} else {
			return;
		}
	}

	if (panningCameraToPaths) {
		if (dWorldCamera_c::instance->panning)
			return;
		panningCameraToPaths = false;
		unlockingAlpha = 0;
	}

	if (unlockingAlpha != -1) {
		unlockingAlpha += 3;

		for (int i = 0; i < pathLayer->pathCount; i++) {
			dKPPath_s *path = pathLayer->paths[i];

			if (path->isAvailable == dKPPath_s::NEWLY_AVAILABLE)
				path->setLayerAlpha(unlockingAlpha);
		}

		for (int i = 0; i < pathLayer->nodeCount; i++) {
			dKPNode_s *node = pathLayer->nodes[i];

			if (node->isNew)
				node->setLayerAlpha(unlockingAlpha);
		}

		if (unlockingAlpha == 255) {
			// we've reached the end
			unlockingAlpha = -1;
			nw4r::snd::SoundHandle something;
			PlaySoundWithFunctionB4(SoundRelatedClass, &something, SE_OBJ_GEN_NEW_COURSE, 1);

			waitAfterUnlock = 15;

			for (int i = 0; i < pathLayer->nodeCount; i++) {
				dKPNode_s *node = pathLayer->nodes[i];

				if (node->isNew && node->type == dKPNode_s::LEVEL) {
					Vec efPos = {node->x, -node->y, 3300.0f};
					S16Vec efRot = {0x2000,0,0};
					Vec efScale = {0.8f,0.8f,0.8f};
					SpawnEffect("Wm_cs_pointlight", 0, &efPos, &efRot, &efScale);
				}
			}
		}

		return;
	}

	if (waitAfterUnlock > 0) {
		waitAfterUnlock--;
		if (waitAfterUnlock == 0)
			waitBeforePanBack = 20;
		return;
	}

	if (waitBeforePanBack > 0) {
		waitBeforePanBack--;
		if (waitBeforePanBack == 0 && camBoundsValid) {
			dWorldCamera_c::instance->panToPosition(
					currentNode->x, -currentNode->y,
					STD_ZOOM);
			panningCameraFromPaths = true;
		}
		return;
	}

	if (panningCameraFromPaths) {
		if (dWorldCamera_c::instance->panning)
			return;
		panningCameraFromPaths = false;
		dWorldCamera_c::instance->followPlayer = true;
	}

	if (dmGladDuration > 0) {
		dmGladDuration--;
		if (dmGladDuration == 60) {
			nw4r::snd::SoundHandle something;
			PlaySoundWithFunctionB4(SoundRelatedClass, &something, SE_VOC_MA_CLEAR_MULTI, 1);
		} else if (dmGladDuration == 0) {
			daWMPlayer_c::instance->startAnimation(wait_select, 1.0f, 0.0f, 0.0f);
		}
		return;
	}

	if (completionAnimDelay > 0) {
		completionAnimDelay--;
		if (completionAnimDelay == 0)
			completionMessagePending = true;
		return;
	}

	// just in case
	if (completionMessagePending)
		return;

	if (completionMessageType > 0) {
		OSReport("We have a completion message type: %d\n", completionMessageType);

		int whichSound;
		if (completionMessageType == CMP_MSG_GLOBAL_COINS) {
			whichSound = STRM_BGM_STAR_COIN_CMPLT_ALL;
			completionAnimDelay = 240 - 154;
		} else if (completionMessageType == CMP_MSG_EVERYTHING) {
			whichSound = STRM_BGM_ALL_CMPLT_5STARS;
			completionAnimDelay = 216 - 154;
		} else {
			whichSound = STRM_BGM_STAR_COIN_CMPLT_WORLD;
			completionAnimDelay = 1;//138;
		}

		nw4r::snd::SoundHandle something;
		PlaySoundWithFunctionB4(SoundRelatedClass, &something, whichSound, 1);

		daWMPlayer_c::instance->startAnimation(coin_comp, 1.0f, 0.0f, 0.0f);
		dmGladDuration = 154;

		return;
	}

	if (dScKoopatlas_c::instance->isEndingScene) {
		// WE GO NO FURTHER
		if (!waitingForEndingTransition) {
			if (!savingForEnding) {
				savingForEnding = true;

				SaveBlock *save = GetSaveFile()->GetBlock(-1);
				save->titleScreenWorld = 3;
				save->titleScreenLevel = 9;

				SaveGame(0, false);
			} else {
				if (!GetSaveFile()->CheckIfWriting()) {
					StartTitleScreenStage(false, 0);
					waitingForEndingTransition = true;
				}
			}
		}
		return;
	}

	if (shouldRequestSave && !checkedForMoveAfterEndLevel) {
		checkedForMoveAfterEndLevel = true;

		static const int endLevels[11][3] = {
			{1, 38, 1}, // W1 right
			{2, 38, 2}, // W2 up
			{3, 38, 0}, // W3 left
			{4, 38, 1}, // W4 right
			{5, 38, 1}, // W5 right
			{6, 38, 2}, // W6 up
			{7, 38, 1}, // W7 right
			{10, 5, 2}, // GW up
			{10, 10, 0}, // MM left
			{10, 15, 3}, // CC down
			{10, 25, 1}, // SC right
		};

		int w = (LastLevelPlayed[0] & 0x7F) + 1;
		int l = LastLevelPlayed[1] + 1;

		for (int i = 0; i < 11; i++) {
			if (endLevels[i][0] == w && endLevels[i][1] == l) {
				afterFortressMode = true;
				startMovementTo(currentNode->exits[endLevels[i][2]]);
				return;
			}
		}
	}

	if (afterFortressMode) {
		if (isMoving)
			moveThroughPath(-1);
		else
			afterFortressMode = false;
		return;
	}

	if (shouldRequestSave) {
		dScKoopatlas_c::instance->showSaveWindow();
		shouldRequestSave = false;
		return;
	}

	if (!initialLoading) {
		dScKoopatlas_c::instance->startMusic();
		dWMHud_c::instance->loadInitially();
		initialLoading = true;
		return;
	}

	int nowPressed = Remocon_GetPressed(GetActiveRemocon());

	// Left, right, up, down
	int pressedDir = getPressedDir(nowPressed);

	if (isMoving) {
		moveThroughPath(pressedDir);
	} else {
		if (pressedDir >= 0) {
			// Use an exit if possible
			if (canUseExit(currentNode->exits[pressedDir])) {
				startMovementTo(currentNode->exits[pressedDir]);
			} else {
				// TODO: maybe remove this? got to see how it looks
				static u16 directions[] = {-0x4000,0x4000,-0x7FFF,0};
				daWMPlayer_c::instance->setTargetRotY(directions[pressedDir]);
			}
		} else if (nowPressed & WPAD_TWO) {
			activatePoint();
		}
	}
}


void dWMPathManager_c::startMovementTo(dKPPath_s *path) {
	SpammyReport("moving to path %p [%d,%d to %d,%d]\n", path, path->start->x, path->start->y, path->end->x, path->end->y);

	if (!path->isAvailable) { return; }
	if (currentNode && dWMHud_c::instance)
		dWMHud_c::instance->leftNode();

	calledEnteredNode = false;

	isMoving = true;
	reverseThroughPath = (path->end == currentNode);

	currentPath = path;

	// calculate direction of the path
	short deltaX = path->end->x - path->start->x;
	short deltaY = path->end->y - path->start->y;
	u16 direction = (u16)(atan2(deltaX, deltaY) / ((M_PI * 2) / 65536.0));

	if (reverseThroughPath) {
		direction += 0x8000;
	}

	daWMPlayer_c *player = daWMPlayer_c::instance;


	// Consider adding these as options
	// wall_walk_l = 60,
	// wall_walk_r = 61,

	// hang_walk_l = 65,
	// hang_walk_r = 66,

	static const struct {
		PlayerAnim anim;
		float animParam1, animParam2;
		s16 forceRotation;
		float forceSpeed;
		SFX repeatSound, initialSound;
		const char *repeatEffect, *initialEffect;
	} Animations[] = {
		// Walking
		{run,2.0f,10.0f, -1,-1.0f, SE_PLY_FOOTNOTE_DIRT,SE_NULL, 0,0},
		{run,2.0f,10.0f, -1,-1.0f, SE_PLY_FOOTNOTE_CS_SAND,SE_NULL, "Wm_mr_foot_sand",0},
		{run,2.0f,10.0f, -1,-1.0f, SE_PLY_FOOTNOTE_CS_SNOW,SE_NULL, "Wm_mr_foot_snow",0},
		{run,2.0f,10.0f, -1,-1.0f, SE_PLY_FOOTNOTE_CS_WATER,SE_NULL, "Wm_mr_foot_water",0},

		// Jumping
		{jump,1.0f,1.0f, -1,2.5f, SE_NULL,SE_PLY_JUMP, 0,0},
		{jump,1.0f,10.0f, -1,2.5f, SE_NULL,SE_PLY_JUMP, 0,0},
		{jump,1.0f,10.0f, -1,2.5f, SE_NULL,SE_PLY_JUMP, 0,0},

		// Jump water (actually cannon)
		{dm_notice,1.0f,10.0f, -1,-1.0f, SE_NULL,SE_VOC_MA_CANNON_SHOT, 0,0},

		// Ladder up, left, right
		{pea_plant,1.2f,10.0f, -0x7FFF,1.5f, SE_PLY_FOOTNOTE_CS_ROCK_CLIMB,SE_NULL, 0,0},
		{tree_climb,1.2f,10.0f, -0x4000,1.5f, SE_PLY_FOOTNOTE_CS_ROCK_CLIMB,SE_NULL, 0,0},
		{tree_climb,1.2f,10.0f, 0x4000,1.5f, SE_PLY_FOOTNOTE_CS_ROCK_CLIMB,SE_NULL, 0,0},
		// Fall (default?)
		{run,2.0f,10.0f, -1,-1.0f, SE_PLY_FOOTNOTE_DIRT,SE_NULL, 0,0},

		// Swim
		{swim_wait,1.2f,10.0f, -1,2.0f, SE_PLY_SWIM,SE_NULL, "Wm_mr_waterswim",0},
		// Run
		{b_dash2,3.0f,10.0f, -1,5.0f, SE_PLY_FOOTNOTE_DIRT,SE_NULL, 0,0},
		// Pipe
		{wait,2.0f,10.0f, 0,1.0f, SE_NULL,SE_PLY_DOKAN_IN_OUT, 0,0},
		// Door
		{wait,2.0f,10.0f, -0x7FFF,0.2f, SE_NULL,SE_OBJ_DOOR_OPEN, 0,0},

		// TJumped
		{Tjumped,2.0f,0.0f, -1,-1.0f, SE_NULL,SE_NULL, 0,0},

		// Enter cave, this is handled specially
		{run,1.0f,10.0f, -1,1.0f, SE_NULL,SE_NULL, 0,0},

		// Cannon 2
		{dm_noti_wait,1.0f,10.0f, -1,-1.0f, SE_NULL,SE_VOC_MA_CANNON_SHOT, 0,0},

		// Invisible, this is handled specially
		{wait,2.0f,10.0f, -1,1.0f, SE_NULL,SE_NULL, 0,0},
	};

	isJumping = (path->animation >= dKPPath_s::JUMP && path->animation <= dKPPath_s::JUMP_SAND);

	float playerScale = 1.6f;

	if (path->animation == dKPPath_s::ENTER_CAVE_UP) {
		scaleAnimProgress = 60;
		// what direction does this path go in?
		static u16 directions[] = {-0x4000,0x4000,-0x7FFF,0};
		isScalingUp = (deltaY < 0) ^ reverseThroughPath;

		if (!isScalingUp)
			playerScale = 0.0f;
	} else if (scaleAnimProgress >= 0) {
		// Keep the current scale
		playerScale = player->scale.x;
	}

	player->visible = (path->animation != dKPPath_s::INVISIBLE);
	player->scale.x = player->scale.y = player->scale.z = playerScale;

	int id = (path->animation >= dKPPath_s::MAX_ANIM) ? 0 : (int)path->animation;
	int whichAnim = Animations[id].anim;
	float updateRate = Animations[id].animParam1;
	if (whichAnim == swim_wait) {
		if (player->modelHandler->mdlClass->powerup_id == 3) {
			whichAnim = b_dash;
			updateRate = 2.5f;
		} else if (player->modelHandler->mdlClass->powerup_id == 5)
			whichAnim = P_slip;
	}

	if (Animations[id].forceRotation != -1) {
		forcedRotation = true;
		player->setTargetRotY(Animations[id].forceRotation);
	} else if (id == dKPPath_s::JUMP_WATER || id == dKPPath_s::RESERVED_18) {
		// keep the current rotation
		forcedRotation = true;
		PlayerAnimStates[dm_notice].playsOnce = 0;

		nw4r::snd::SoundHandle something;
		PlaySoundWithFunctionB4(SoundRelatedClass, &something, SE_OBJ_WARP_CANNON_SHOT, 1);

		dWMMap_c::instance->spinLaunchStar();
	} else {
		forcedRotation = false;
		player->setTargetRotY(direction);
	}

	player->startAnimation(whichAnim, updateRate, Animations[id].animParam2, 0.0f);

	moveSpeed = (Animations[id].forceSpeed >= 0.0f) ? Animations[id].forceSpeed : 3.0f;
	moveSpeed = path->speed * moveSpeed * 1.3f;
	if (path->animation == dKPPath_s::SWIM) {
		if (player->modelHandler->mdlClass->powerup_id == 3)
			moveSpeed *= 1.1f;
		else if (player->modelHandler->mdlClass->powerup_id == 5)
			moveSpeed *= 2.0f;
	}

	if (Animations[id].repeatEffect) {
		player->hasEffect = true;
		player->effectName = Animations[id].repeatEffect;
	} else {
		player->hasEffect = false;
	}

	if (Animations[id].repeatSound != SE_NULL) {
		player->hasSound = true;
		player->soundName = Animations[id].repeatSound;
	} else {
		player->hasSound = false;
	}

	if (Animations[id].initialEffect) {
		SpawnEffect(Animations[id].initialEffect, 0, &player->pos, 0, &player->scale);
	}

	if (path->animation == dKPPath_s::SWIM) {
		if (player->modelHandler->mdlClass->powerup_id == 5) {
			if (!swimming) {
				nw4r::snd::SoundHandle something;
				if (firstPathDone)
					PlaySoundWithFunctionB4(SoundRelatedClass, &something, SE_VOC_MA_PNGN_SLIDE, 1);
				PlaySoundWithFunctionB4(SoundRelatedClass, &penguinSlideSound, SE_EMY_PENGUIN_SLIDE, 1);
			}
			player->hasSound = false;
		} else if (player->modelHandler->mdlClass->powerup_id == 3) {
			player->hasSound = true;
			player->soundName = SE_PLY_FOOTNOTE_WATER;
		}
		swimming = true;
	} else {
		if (swimming && penguinSlideSound.Exists()) {
			penguinSlideSound.Stop(10);
			penguinSlideSound.DetachSound();
		}
		swimming = false;

		if (Animations[id].initialSound != SE_NULL) {
			nw4r::snd::SoundHandle something;
			PlaySoundWithFunctionB4(SoundRelatedClass, &something, Animations[id].initialSound, 1);

			if (Animations[id].initialSound == SE_PLY_JUMP) {
				nw4r::snd::SoundHandle something2;
				PlaySoundWithFunctionB4(SoundRelatedClass, &something2, SE_VOC_MA_CS_JUMP, 1);
				something2.SetPitch(player->modelHandler->mdlClass->powerup_id == 3 ? 1.5f : 1.0f);
			}
		}
	}
}

void dWMPathManager_c::moveThroughPath(int pressedDir) {
	dKPNode_s *from, *to;

	from = reverseThroughPath ? currentPath->end : currentPath->start;
	to = reverseThroughPath ? currentPath->start : currentPath->end;

	daWMPlayer_c *player = daWMPlayer_c::instance;

	if (pressedDir >= 0 && !calledEnteredNode) {
		int whatDirDegrees = ((int)(atan2(to->x-from->x, to->y-from->y) / ((M_PI * 2) / 360.0)) + 360) % 360;
		// dirs are: left, right, up, down
		int whatDir;
		if (whatDirDegrees >= 225 && whatDirDegrees <= 315)
			whatDir = 1; // moving Left, so reversing requires Right
		else if (whatDirDegrees >= 45 && whatDirDegrees <= 135)
			whatDir = 0; // moving Right, so reversing requires Left
		else if (whatDirDegrees > 135 && whatDirDegrees < 225)
			whatDir = 3; // moving Up, so reversing requires Down
		else if (whatDirDegrees > 315 || whatDirDegrees < 45)
			whatDir = 2; // moving Down, so reversing requires Up
		OSReport("Delta: %d, %d; Degrees: %d (Atan result is %f); Calced dir is %d; Pressed dir is %d\n", to->x-from->x, to->y-from->y, whatDirDegrees, atan2(to->x-from->x,to->y-from->y), whatDir, pressedDir);

		if (whatDir == pressedDir) {
			// are we using a forbidden animation?
			static const dKPPath_s::Animation forbidden[] = {
				dKPPath_s::JUMP, dKPPath_s::JUMP_SAND,
				dKPPath_s::JUMP_SNOW, dKPPath_s::JUMP_WATER,
				dKPPath_s::PIPE, dKPPath_s::DOOR,
				dKPPath_s::ENTER_CAVE_UP, dKPPath_s::INVISIBLE,
				dKPPath_s::RESERVED_18, dKPPath_s::MAX_ANIM
			};
			bool allowed = true;
			for (int i = 0;;i++) {
				if (forbidden[i] == dKPPath_s::MAX_ANIM)
					break;
				if (forbidden[i] == currentPath->animation)
					allowed = false;
			}

			if (allowed) {
				reverseThroughPath = !reverseThroughPath;
				if (!forcedRotation)
					player->setTargetRotY(player->targetRotY + 0x8000);
				// start over with the reversed path!
				moveThroughPath(-1);
				return;
			}
		}
	}


	if (scaleAnimProgress >= 0) {
		float soFar = scaleAnimProgress * (1.6f / 60.0f);
		float sc = isScalingUp ? soFar : (1.6f - soFar);
		player->scale.x = player->scale.y = player->scale.z = sc;

		scaleAnimProgress--;
	}


	Vec move = (Vec){to->x - from->x, to->y - from->y, 0};
	VECNormalize(&move, &move);
	VECScale(&move, &move, moveSpeed);

	if (isJumping) {
		bool isFalling;
		if (from->y == to->y) {
			float xDistance = to->x - from->x;
			if (xDistance < 0)
				xDistance = -xDistance;
			float currentPoint = max(to->x, from->x) - player->pos.x;
			player->jumpOffset = (xDistance / 3.0f) * sin((currentPoint / xDistance) * 3.1415f);

			if (to->x > from->x) // Moving right
				isFalling = (player->pos.x > (to->x - (move.x * 10.0f)));
			else // Moving left
				isFalling = (player->pos.x < (to->x - (move.x * 10.0f)));

		} else {
			float ys = (float)from->y;
			float ye = (float)to->y;
			float midpoint = (from->y + to->y) / 2;

			float top, len;
			if (ys > ye) { len = ys - ye; top = ys - midpoint + 10.0; }
			else		 { len = ye - ys; top = ye - midpoint + 10.0; }

			if (len == 0.0) { len = 2.0; }

			float a;
			if (timer > 0.0) { a = -timer; }
			else			 { a =  timer; }


			player->jumpOffset = -sin(a * 3.14 / len) * top;
			timer -= move.y;

			if (ye > ys) // Moving down
				isFalling = (-player->pos.y) > (ye - (move.y * 10.0f));
			else // Moving up
				isFalling = (-player->pos.y) < (ye - (move.y * 10.0f));
		}
		if (isFalling)
			player->startAnimation(jumped, 1.0f, 10.0f, 0.0f);
	}

	player->pos.x += move.x;
	player->pos.y -= move.y;

	// what distance is left?
	if (to->type == dKPNode_s::LEVEL && !calledEnteredNode) {
		Vec toEndVec = {to->x - player->pos.x, to->y + player->pos.y, 0.0f};
		float distToEnd = VECMag(&toEndVec);
		//OSReport("Distance: %f; To:%d,%d; Player:%f,%f; Diff:%f,%f\n", distToEnd, to->x, to->y, player->pos.x, player->pos.y, toEndVec.x, toEndVec.y);

		if (distToEnd < 64.0f && dWMHud_c::instance) {
			calledEnteredNode = true;
			dWMHud_c::instance->enteredNode(to);
		}
	}

	// Check if we've reached the end yet
	if (
			(((move.x > 0) ? (player->pos.x >= to->x) : (player->pos.x <= to->x)) &&
			 ((move.y > 0) ? (-player->pos.y >= to->y) : (-player->pos.y <= to->y)))
			||
			(from->x == to->x && from->y == to->y)
	   ) {

		currentNode = to;
		player->pos.x = to->x;
		player->pos.y = -to->y;

		isJumping = false;
		timer = 0.0;

		SpammyReport("reached path end (%p) with type %d\n", to, to->type);

		bool reallyStop = false;

		if (to->type == dKPNode_s::LEVEL) {
			// Always stop on levels
			reallyStop = true;
		} else if (to->type == dKPNode_s::CHANGE || to->type == dKPNode_s::WORLD_CHANGE || to->type == dKPNode_s::PASS_THROUGH) {
			// If there's only one exit here, then stop even though
			// it's a passthrough node
			reallyStop = (to->getAvailableExitCount() == 1);
		} else {
			// Quick check: do we *actually* need to stop on this node?
			// If it's a junction with more than two exits, but only two are open,
			// take the opposite open one
			if (!dScKoopatlas_c::instance->warpZoneHacks && to->getExitCount() > 2 && to->getAvailableExitCount() == 2)
				reallyStop = false;
			else
				reallyStop = true;
		}

		if (to->type == dKPNode_s::WORLD_CHANGE) {
			// Set the current world info
			SaveBlock *save = GetSaveFile()->GetBlock(-1);

			OSReport("Activating world change %d\n", to->worldID);
			const dKPWorldDef_s *world = dScKoopatlas_c::instance->mapData.findWorldDef(to->worldID);
			if (world) {
				bool visiblyChange = true;
				if (strncmp(save->newerWorldName, world->name, 32) == 0) {
					OSReport("Already here, but setting BGM track\n");
					visiblyChange = false;
				}

				OSReport("Found!\n");
				copyWorldDefToSave(world);

				bool wzHack = false;
				if (dScKoopatlas_c::instance->warpZoneHacks) {
					save->hudHintH += 1000;

					if (world->worldID > 0) {
						dLevelInfo_c *linfo = &dLevelInfo_c::s_info;
						dLevelInfo_c::entry_s *lastLevel;
						if (world->worldID == 0)
							lastLevel = linfo->searchByDisplayNum(1, 27);
						else if (world->worldID != 7)
							lastLevel = linfo->searchByDisplayNum(world->worldID-1, lastLevelIDs[world->worldID-1]);
						else
							lastLevel = linfo->searchByDisplayNum(7, 3);

						if (lastLevel) {
							wzHack = !(save->GetLevelCondition(lastLevel->worldSlot,lastLevel->levelSlot) & COND_NORMAL);
						}

						// another stupid thing
						if (world->worldID == 7 && wzHack)
							if (save->GetLevelCondition(9,24) & COND_NORMAL)
								wzHack = false;
					}
				}

				if (wzHack) {
					save->hudHintH = 2000;
					dWMHud_c::instance->hideFooter();
				} else {
					if (visiblyChange && dWMHud_c::instance)
						dWMHud_c::instance->showFooter();
				}

				dKPMusic::play(world->trackID);

			} else if (to->worldID == 0) {
				OSReport("No world\n");
				save->currentMapMusic = 0;
				dKPMusic::play(0);
				save->newerWorldName[0] = 0;
				if (dWMHud_c::instance)
					dWMHud_c::instance->hideFooter();
			} else {
				OSReport("Not found!\n");
			}
		}

		if (to->type == dKPNode_s::CHANGE) {
			// Go to another map

			// should we continue moving?
			if (to->getAvailableExitCount() == 1) {
				OSReport("Stopping");
				isMoving = false;
			} else {
				OSReport("Continuing");
				startMovementTo(to->getOppositeAvailableExitTo(currentPath));
			}

			SaveBlock *save = GetSaveFile()->GetBlock(-1);
			SpammyReport("node: %x, %s", to->destMap, to->destMap);
			save->current_world = dScKoopatlas_c::instance->getIndexForMapName(to->destMap);

			SpammyReport("Change to map ID %d (%s), entrance ID %d\n", save->current_world, to->destMap, to->foreignID);

			dScKoopatlas_c::instance->keepMusicPlaying = true;
			ActivateWipe(to->transition);
			u32 saveFlag = (shouldRequestSave ? 0x80000 : 0);
			saveFlag |= (checkedForMoveAfterEndLevel ? 0x40000 : 0);
			saveFlag |= (afterFortressMode ? 0x20000 : 0);
			DoSceneChange(WORLD_MAP, 0x10000000 | (to->foreignID << 20) | saveFlag, 0);

		} else if (reallyStop) {
			// Stop here
			bool movingAgain = false;
			player->hasEffect = false;
			player->hasSound = false;

			SpammyReport("stopping here\n");

			isMoving = false;
			swimming = false;

			SaveBlock *save = GetSaveFile()->GetBlock(-1);
			save->current_path_node = pathLayer->findNodeID(to);
			if (!calledEnteredNode && dWMHud_c::instance)
				dWMHud_c::instance->enteredNode();

			// Should we continue here? (Requested by Jason)
			int held = Remocon_GetButtons(GetActiveRemocon());
			int pressedDir = getPressedDir(held);
			if (pressedDir >= 0) {
				// Use an exit if possible
				if (canUseExit(currentNode->exits[pressedDir])) {
					startMovementTo(currentNode->exits[pressedDir]);
					movingAgain = true;
				} else {
					// TODO: maybe remove this? got to see how it looks
					static u16 directions[] = {-0x4000,0x4000,-0x7FFF,0};
					daWMPlayer_c::instance->setTargetRotY(directions[pressedDir]);
				}
			}

			if (!movingAgain)
				player->startAnimation(wait_select, 1.2, 10.0, 0.0);

		} else {
			startMovementTo(to->getOppositeAvailableExitTo(currentPath));
			SpammyReport("passthrough node, continuing to next path\n");
		}
	}

	firstPathDone = true;
}

void dWMPathManager_c::copyWorldDefToSave(const dKPWorldDef_s *world) {
	SaveBlock *save = GetSaveFile()->GetBlock(-1);

	strncpy(save->newerWorldName, world->name, 32);
	save->newerWorldName[31] = 0;
	save->newerWorldID = world->worldID;
	save->currentMapMusic = world->trackID;

	for (int i = 0; i < 2; i++) {
		save->fsTextColours[i] = world->fsTextColours[i];
		save->fsHintColours[i] = world->fsHintColours[i];
		save->hudTextColours[i] = world->hudTextColours[i];
	}

	save->hudHintH = world->hudHintH;
	save->hudHintS = world->hudHintS;
	save->hudHintL = world->hudHintL;

	if (save->titleScreenWorld == 3 && save->titleScreenLevel == 10)
		return;
	save->titleScreenWorld = world->titleScreenWorld;
	save->titleScreenLevel = world->titleScreenLevel;
}

void dWMPathManager_c::activatePoint() {
	if (levelStartWait >= 0)
		return;

	if (currentNode->type == dKPNode_s::LEVEL) {
		int w = currentNode->levelNumber[0] - 1;
		int l = currentNode->levelNumber[1] - 1;

		if (l == 98) {
			dWMShop_c::instance->show(w);
			dWMHud_c::instance->hideAll();
			dScKoopatlas_c::instance->state.setState(&dScKoopatlas_c::instance->StateID_ShopWait);
			return;
		}

		if ((l >= 29) && (l <= 36)) {
			SaveBlock *save = GetSaveFile()->GetBlock(-1);
			u32 conds = save->GetLevelCondition(w, l);

			SpammyReport("Toad House Flags: %x", conds);
			if (conds & 0x30) { 
				nw4r::snd::SoundHandle something;
				PlaySoundWithFunctionB4(SoundRelatedClass, &something, SE_SYS_INVALID, 1);
				return;
			}
		}

		nw4r::snd::SoundHandle something;
		PlaySoundWithFunctionB4(SoundRelatedClass, &something, SE_SYS_GAME_START, 1);

		nw4r::snd::SoundHandle something2;
		PlaySoundWithFunctionB4(SoundRelatedClass, &something2, (Player_Powerup[0] == 3) ? SE_VOC_MA_PLAYER_DECIDE_MAME: SE_VOC_MA_CS_COURSE_IN, 1);

		daWMPlayer_c::instance->startAnimation(course_in, 1.2, 10.0, 0.0);
		daWMPlayer_c::instance->setTargetRotY(0);

		isEnteringLevel = true;
		levelStartWait = 40;
		enteredLevel = dLevelInfo_c::s_info.searchBySlot(w, l);

		dKPMusic::stop();
	}
}


void dWMPathManager_c::unlockAllPaths(char type) {

	if (type == 0 || type == 1) {
		SaveBlock *save = GetSaveFile()->GetBlock(-1);
		for (int j = 0; j < 10; j++) {
			for (int h = 0; h < 0x2A; h++) {
				if (j != 1 || h != 9)
					save->completions[j][h] = (type == 1) ? 0x17 : 0x37;
			}
		}
		unlockPaths();
	}

	// Unlocks current path, regular and secret
	if (type == 2) {
		if (currentNode->type == dKPNode_s::LEVEL) {
			int w = currentNode->levelNumber[0] - 1;
			int l = currentNode->levelNumber[1] - 1;

			SaveBlock *save = GetSaveFile()->GetBlock(-1);
			save->completions[w][l] = 0x37;
			unlockPaths();
		}
	}

	// Can't change node models - the price we pay for not using anims
	// for (int i = 0; i < pathLayer->nodeCount; i++) {
	// 	dKPNode_s *node = pathLayer->nodes[i];
	// 	node->setupNodeExtra();
	// }

}




void dWMPathManager_c::findCameraBoundsForUnlockedPaths() {
	dKPMapData_c *data = &dScKoopatlas_c::instance->mapData;

	camMinX = 10000;
	camMaxX = 0;
	camMinY = 10000;
	camMaxY = 0;

	nodeStackLength = 0;
	for (int i = 0; i < data->pathLayer->nodeCount; i++)
		data->pathLayer->nodes[i]->reserved1 = false;

	visitNodeForCamCheck(currentNode);
	OSReport("Worked out camera bounds: %d,%d to %d,%d with validity %d\n", camMinX, camMinY, camMaxX, camMaxY, camBoundsValid);
}

void dWMPathManager_c::visitNodeForCamCheck(dKPNode_s *node) {
	// Yay.
	nodeStackLength++;
	node->reserved1 = true;

	for (int i = 0; i < 4; i++) {
		dKPPath_s *path = node->exits[i];
		if (!path)
			continue;

		OSReport("Checking path %p, whose status is %d\n", path, path->isAvailable);
		if (path->isAvailable == dKPPath_s::NEWLY_AVAILABLE) {
			addNodeToCameraBounds(path->start);
			addNodeToCameraBounds(path->end);
		}

		// Should we follow the other node?
		dKPNode_s *otherNode = path->getOtherNodeTo(node);

		if (otherNode->reserved1)
			continue;
		if (otherNode->type == otherNode->LEVEL) {
			OSReport("Not travelling to %p because it's level %d-%d\n", otherNode, otherNode->levelNumber[0], otherNode->levelNumber[1]);
			continue;
		}
		if (otherNode->type == otherNode->CHANGE) {
			OSReport("Not travelling to %p because it's a change\n", otherNode);
			continue;
		}
		if (otherNode->type == otherNode->WORLD_CHANGE) {
			OSReport("Not travelling to %p because it's a world change\n", otherNode);
			continue;
		}

		visitNodeForCamCheck(otherNode);
	}

	nodeStackLength--;
}

void dWMPathManager_c::addNodeToCameraBounds(dKPNode_s *node) {
	camBoundsValid = true;
	OSReport("Adding node to camera bounds: %p at %d,%d\n", node, node->x, node->y);

	if (node->x < camMinX)
		camMinX = node->x;
	if (node->x > camMaxX)
		camMaxX = node->x;
	if (node->y < camMinY)
		camMinY = node->y;
	if (node->y > camMaxY)
		camMaxY = node->y;
}


//
// processed\../src/koopatlas/shop.cpp
//

#include "koopatlas/shop.h"

CREATE_STATE(dWMShop_c, Hidden);
CREATE_STATE(dWMShop_c, ShowWait);
CREATE_STATE(dWMShop_c, ButtonActivateWait);
CREATE_STATE(dWMShop_c, CoinCountdown);
CREATE_STATE(dWMShop_c, Wait);
CREATE_STATE(dWMShop_c, HideWait);

void dWMShop_c::ShopModel_c::setupItem(float x, float y, ItemTypes type) {
	static const char* Produce[ITEM_TYPE_COUNT][4] = { 
		{ "I_kinoko", 		"g3d/I_kinoko.brres", 			"I_kinoko", 			"wait2" },
		{ "I_fireflower", 	"g3d/I_fireflower.brres", 		"I_fireflower", 		"wait2" },
		{ "I_propeller", 	"g3d/I_propeller.brres", 		"I_propeller_model", 	"wait2" },
		{ "I_iceflower", 	"g3d/I_iceflower.brres", 		"I_iceflower", 			"wait2" },
		{ "I_penguin", 		"g3d/I_penguin.brres", 			"I_penguin", 			"wait2" },
		{ "I_kinoko_bundle","g3d/I_mini_kinoko.brres", 		"I_mini_kinoko", 		"wait2" },
		{ "I_star", 		"g3d/I_star.brres", 			"I_star", 				"wait2" },
		{ "I_hammer", 		"g3d/I_fireflower.brres",		"I_fireflower",			"wait2" },
		{ "I_kinoko_bundle","g3d/I_life_kinoko.brres", 		"I_life_kinoko", 		"wait2" },
	};

	this->x = x;
	this->y = y;
	scaleFactor = 2.3f;

	int id = (int)type;
	isLakitu = false;

	allocator.link(-1, GameHeaps[0], 0, 0x20);

	res.data = getResource(Produce[id][0], Produce[id][1]);
	nw4r::g3d::ResMdl mdlRes = res.GetResMdl(Produce[id][2]);
	model.setup(mdlRes, &allocator, 0x224, 1, 0);

	SetupTextures_Item(&model, 1);

	nw4r::g3d::ResAnmChr anmChr = res.GetResAnmChr(Produce[id][3]);
	animation.setup(mdlRes, anmChr, &allocator, 0);
	playAnim(Produce[id][3], 1.0f, 0);

	allocator.unlink();
}

void dWMShop_c::ShopModel_c::setupLakitu(int id) {
	static const char* models[10] = { 
		"g3d/yoshi.brres", "g3d/desert.brres", "g3d/mountain.brres", "g3d/sakura.brres", "g3d/santa.brres", 
		"g3d/ghost.brres", "g3d/space.brres", "g3d/koopa.brres", "g3d/sewer.brres", "g3d/goldwood.brres" 
	};

	scaleFactor = 1.0f;
	scaleEase = 0.0f;
	this->isLakitu = true;

	allocator.link(-1, GameHeaps[0], 0, 0x20);

	res.data = getResource("lakitu", models[id]);
	nw4r::g3d::ResMdl mdlRes = res.GetResMdl("lakitu");
	model.setup(mdlRes, &allocator, 0x224, 1, 0);
	SetupTextures_Enemy(&model, 1);

	nw4r::g3d::ResAnmChr anmChr = res.GetResAnmChr("idle");
	animation.setup(mdlRes, anmChr, &allocator, 0);

	playAnim("idle", 1.0f, 0);
	playingNotEnough = false;

	allocator.unlink();
}

void dWMShop_c::ShopModel_c::playAnim(const char *name, float rate, char loop) {
	nw4r::g3d::ResAnmChr anmChr = res.GetResAnmChr(name);
	animation.bind(&model, anmChr, loop);
	model.bindAnim(&animation, 0.0f);
	animation.setUpdateRate(rate);
}

void dWMShop_c::ShopModel_c::execute() {
	model._vf1C();

	if(this->animation.isAnimationDone()) {
		OSReport("Animaiton Complete");
		if (this->isLakitu) {
			OSReport("Setting animation to idle");
			playAnim("idle", 1.0f, 0);
			this->animation.setCurrentFrame(0.0);
			if (playingNotEnough) {
				OSReport("Detected Not Enough animation as being over\n");
				playingNotEnough = false;
			}
		}
	}	
}

void dWMShop_c::ShopModel_c::draw() {
	mMtx mtx;
	mtx.translation(x, y, 1000.0f);
	model.setDrawMatrix(mtx);

	float s = scaleFactor * scaleEase;

	Vec scale = {s, s, s};
	model.setScale(&scale);

	model.calcWorld(false);
	model.scheduleForDrawing();
}



dWMShop_c *dWMShop_c::instance = 0;

dWMShop_c *dWMShop_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(dWMShop_c));
	dWMShop_c *c = new(buffer) dWMShop_c;

	instance = c;
	return c;
}

dWMShop_c::dWMShop_c() : state(this, &StateID_Hidden) {
	layoutLoaded = false;
	visible = false;
}

int dWMShop_c::onCreate() {
	if (!layoutLoaded) {
		bool gotFile = layout.loadArc("shop.arc", false);
		if (!gotFile)
			return false;

		selected = 0;
		lastTopRowChoice = 0;

		layout.build("shop.brlyt");

		if (IsWideScreen()) {
			layout.layout.rootPane->scale.x = 0.735f;
		} else {
			layout.clippingEnabled = true;
			layout.clipX = 0;
			layout.clipY = 52;
			layout.clipWidth = 640;
			layout.clipHeight = 352;
			layout.layout.rootPane->scale.x = 0.731f;
			layout.layout.rootPane->scale.y = 0.7711f;
		}

		static const char *brlanNames[] = {
			"shop_Show.brlan",
			"shop_Hide.brlan",
			"shop_ActivateButton.brlan",
			"shop_DeactivateButton.brlan",
			"shop_CountCoin.brlan"
		};
		static const char *groupNames[] = {
			"BaseGroup", "BaseGroup",
			"GBtn00", "GBtn01", "GBtn02", "GBtn03", "GBtn1", "GBtn2",
			"GBtn00", "GBtn01", "GBtn02", "GBtn03", "GBtn1", "GBtn2",
			"GCoinCount"
		};
		static const int brlanIDs[] = {
			0, 1,
			2, 2, 2, 2, 2, 2,
			3, 3, 3, 3, 3, 3,
			4,
		};

		layout.loadAnimations(brlanNames, 5);
		layout.loadGroups(groupNames, brlanIDs, 15);
		layout.disableAllAnimations();

		layout.drawOrder = 1;

		static const char *tbNames[] = {
			"Title", "TitleShadow",
			"CoinCount", "CoinCountShadow",
			"BackText", "BuyText",
		};
		layout.getTextBoxes(tbNames, &Title, 6);

		// Warning: weird code coming up
		const char *crap = "000102031\0" "2\0";
		char name[12];
		for (int i = 0; i < 6; i++) {
			strcpy(name, "BtnLeftXX");
			name[7] = crap[i*2];
			name[8] = crap[i*2+1];
			BtnLeft[i] = layout.findPictureByName(name);

			strcpy(name, "BtnMidXX");
			name[6] = crap[i*2];
			name[7] = crap[i*2+1];
			BtnMid[i] = layout.findPictureByName(name);

			strcpy(name, "BtnRightXX");
			name[8] = crap[i*2];
			name[9] = crap[i*2+1];
			BtnRight[i] = layout.findPictureByName(name);

			strcpy(name, "BtnXX");
			name[3] = crap[i*2];
			name[4] = crap[i*2+1];
			Buttons[i] = layout.findPaneByName(name);
		}

		Btn1Base = layout.findPaneByName("Btn1_Base");
		Btn2Base = layout.findPaneByName("Btn2_Base");
		OSReport("Found btn 1,2: %p, %p\n", Btn1Base, Btn2Base);

		leftCol.setTexMap(BtnLeft[0]->material->texMaps);
		midCol.setTexMap(BtnMid[0]->material->texMaps);
		rightCol.setTexMap(BtnRight[0]->material->texMaps);

		for (int i = 1; i < 6; i++) {
			leftCol.applyAlso(BtnLeft[i]->material->texMaps);
			midCol.applyAlso(BtnMid[i]->material->texMaps);
			rightCol.applyAlso(BtnRight[i]->material->texMaps);
		}

		layoutLoaded = true;
	}

	return true;
}


int dWMShop_c::onDelete() {
	deleteModels();
	return layout.free();
}


int dWMShop_c::onExecute() {
	state.execute();

	if (visible) {
		lakituModel->execute();

		for (int i = 0; i < 12; i++)
			itemModels[i].execute();
	}

	layout.execAnimations();
	layout.update();

	return true;
}

int dWMShop_c::onDraw() {
	if (visible)
		layout.scheduleForDrawing();

	return true;
}

void dWMShop_c::specialDraw1() {
	if (visible) {
		lakituModel->scaleEase = scaleEase;
		lakituModel->draw();
		for (int i = 0; i < 12; i++) {
			itemModels[i].scaleEase = scaleEase;
			itemModels[i].draw();
		}
	}


//		if (wasOff) { effect.spawn("Wm_ob_greencoinkira", 0, &pos, &rot, &scale); wasOff = false; }

//		if(this->ska.isAnimationDone())
//			this->ska.setCurrentFrame(0.0);
}


void dWMShop_c::show(int shopNumber) {
	shopKind = shopNumber;
	state.setState(&StateID_ShowWait);
}


// Hidden
void dWMShop_c::beginState_Hidden() { }
void dWMShop_c::executeState_Hidden() { }
void dWMShop_c::endState_Hidden() { }

// ShowWait
void dWMShop_c::beginState_ShowWait() {
	MapSoundPlayer(SoundRelatedClass, SE_SYS_DIALOGUE_IN, 1);

	layout.disableAllAnimations();
	layout.enableNonLoopAnim(SHOW_ALL);
	visible = true;
	scaleEase = 0.0;

	loadInfo();
	loadModels();
}
void dWMShop_c::executeState_ShowWait() {
	if (!layout.isAnimOn(SHOW_ALL)) {
		selected = 0;
		layout.enableNonLoopAnim(ACTIVATE_BUTTON);
		state.setState(&StateID_ButtonActivateWait);
	}
}
void dWMShop_c::endState_ShowWait() {
	MapSoundPlayer(SoundRelatedClass, SE_OBJ_CLOUD_BLOCK_TO_JUGEM, 1);
	timer = 1;
}

// ButtonActivateWait
void dWMShop_c::beginState_ButtonActivateWait() { }
void dWMShop_c::executeState_ButtonActivateWait() {
	if (!layout.isAnyAnimOn())
		state.setState(&StateID_Wait);
}
void dWMShop_c::endState_ButtonActivateWait() { }

// Wait
void dWMShop_c::beginState_Wait() {
	showSelectCursor();
}
void dWMShop_c::executeState_Wait() {
	if (timer < 90) {
		scaleEase = -((cos(timer * 3.14 /20)-0.9)/timer*10)+1;
		timer++;
		return;
	}

	int nowPressed = Remocon_GetPressed(GetActiveRemocon());

	int newSelection = -1;

	if (nowPressed & WPAD_ONE) {
		// Hide the thing
		state.setState(&StateID_HideWait);

	} else if (nowPressed & WPAD_UP) {
		// Move up
		if (selected == 4)
			newSelection = lastTopRowChoice;
		else if (selected == 5)
			newSelection = 4;

	} else if (nowPressed & WPAD_DOWN) {
		// Move down
		if (selected <= 3)
			newSelection = 4;
		else if (selected == 4)
			newSelection = 5;

	} else if (nowPressed & WPAD_LEFT) {
		// Just use the previous choice
		if (selected > 0)
			newSelection = selected - 1;

	} else if (nowPressed & WPAD_RIGHT) {
		// Just use the next choice
		if (selected < 5)
			newSelection = selected + 1;

	} else if (nowPressed & WPAD_TWO) {
		if (!lakituModel->playingNotEnough)
			buyItem(selected);
		else
			OSReport("Not Enough is still playing!\n");
	}

	if (newSelection > -1) {
		MapSoundPlayer(SoundRelatedClass, SE_SYS_CURSOR, 1);

		layout.enableNonLoopAnim(DEACTIVATE_BUTTON+selected);
		layout.enableNonLoopAnim(ACTIVATE_BUTTON+newSelection);

		selected = newSelection;
		if (newSelection <= 3)
			lastTopRowChoice = newSelection;

		showSelectCursor();
	}
}
void dWMShop_c::endState_Wait() { }

// HideWait
void dWMShop_c::beginState_HideWait() {
	MapSoundPlayer(SoundRelatedClass, SE_SYS_DIALOGUE_OUT_AUTO, 1);
	layout.enableNonLoopAnim(HIDE_ALL);
	layout.enableNonLoopAnim(DEACTIVATE_BUTTON+selected);

	timer = 26;
	MapSoundPlayer(SoundRelatedClass, SE_OBJ_CS_KINOHOUSE_DISAPP, 1);

	HideSelectCursor(SelectCursorPointer, 0);
}
void dWMShop_c::executeState_HideWait() {
	if (timer > 0) {
		timer--;
		scaleEase = -((cos(timer * 3.14 /13.5)-0.9)/timer*10)+1;
		if (scaleEase < 0.0f)
			scaleEase = 0.0f;
	}

	if (!layout.isAnimOn(HIDE_ALL))
		state.setState(&StateID_Hidden);
}
void dWMShop_c::endState_HideWait() {
	deleteModels();
	visible = false;
}


// Powerup Listing:
// 0 = Mushroom		- 1
	// 1 = Fireflower	- 2
	// 2 = Iceflower	- 2
		// 3 = Penguin		- 3
		// 4 = Propeller	- 3
	// 5 = MiniShroom	- 2
	// 6 = Starman		- 2
		// 7 = Hammer		- 3
	// 8 = 1-ups		- 2
//
// Format: 1coin, 1coin, 2coins, 3coins, 5coins[3] (Value 6-9), 8coins[5] (Value 10-15)
//
// Possible 5 coin combos =  2,2,2  /  1,2,3  /  2,3,2  /  3,2,3  /  3,3,3
// Possible 8 coin combos =  1,1,2,3,3  /  1,2,2,3,3  /  1,2,3,3,3  /  2,2,2,3,3  /  2,2,3,3,3  /  1,3,3,3,3  /  2,3,3,3,3  /  3,3,3,3,3

const dWMShop_c::ItemTypes dWMShop_c::Inventory[10][12] = { 
#if defined(FALLING_LEAF)
	{
		MUSHROOM, FIRE_FLOWER, ICE_FLOWER, PROPELLER,
		FIRE_FLOWER, HAMMER, PROPELLER,
		PENGUIN, MUSHROOM, MINI_SHROOM, FIRE_FLOWER, PROPELLER
	},
#else
	{ // Yoshi's Island
		MUSHROOM, FIRE_FLOWER, ICE_FLOWER, PROPELLER,
		FIRE_FLOWER, ICE_FLOWER, FIRE_FLOWER,
		MUSHROOM, MUSHROOM, ONE_UP, PROPELLER, PROPELLER
	},
#endif
	{ // Desert
		MUSHROOM, FIRE_FLOWER, ICE_FLOWER, PROPELLER,
		FIRE_FLOWER, STARMAN, FIRE_FLOWER,
		MUSHROOM, FIRE_FLOWER, FIRE_FLOWER, PROPELLER, PROPELLER
	},
	{ // Mountain
		MUSHROOM, FIRE_FLOWER, MINI_SHROOM, PROPELLER,
		MUSHROOM, MINI_SHROOM, PROPELLER,
		MUSHROOM, MINI_SHROOM, PROPELLER, PROPELLER, HAMMER
	},
	{ // Japan
		MUSHROOM, FIRE_FLOWER, ONE_UP, HAMMER,
		ONE_UP, ONE_UP, ONE_UP,
		PROPELLER, ICE_FLOWER, ONE_UP, FIRE_FLOWER, PROPELLER
	},
	{ // FreezeFlame
		MUSHROOM, FIRE_FLOWER, ICE_FLOWER, PENGUIN,
		ICE_FLOWER, PENGUIN, ICE_FLOWER,
		ICE_FLOWER, PENGUIN, PENGUIN, PENGUIN, ICE_FLOWER
	},
	{ // Ghost
		MUSHROOM, FIRE_FLOWER, STARMAN, PROPELLER,
		MINI_SHROOM, PROPELLER, MINI_SHROOM,
		PROPELLER, PROPELLER, MUSHROOM, PROPELLER, PROPELLER
	},
	{ // Space
		MUSHROOM, STARMAN, ONE_UP, HAMMER,
		STARMAN, STARMAN, STARMAN,
		HAMMER, HAMMER, ONE_UP, HAMMER, HAMMER
	},
	{ // Koopa
		MUSHROOM, ONE_UP, PROPELLER, HAMMER,
		HAMMER, PROPELLER, HAMMER,
		PROPELLER, HAMMER, PROPELLER, HAMMER, PROPELLER
	},
	{ // Unknown
		MUSHROOM, MUSHROOM, MUSHROOM, MUSHROOM,
		MUSHROOM, MUSHROOM, MUSHROOM,
		MUSHROOM, MUSHROOM, MUSHROOM, MUSHROOM, MUSHROOM
	},
	{ // Goldwood
		MUSHROOM, FIRE_FLOWER, ONE_UP, PENGUIN,
		FIRE_FLOWER, PROPELLER, FIRE_FLOWER,
		FIRE_FLOWER, FIRE_FLOWER, STARMAN, FIRE_FLOWER, FIRE_FLOWER
	}
};

void dWMShop_c::loadModels() {
	lakituModel = new ShopModel_c;
	lakituModel->setupLakitu(shopKind);
	lakituModel->x = 240.0f;
	lakituModel->y = 220.0f;
	if (!IsWideScreen()) {
		lakituModel->x = (0.731f * (lakituModel->x + 416.0f)) - 292.0f;
		lakituModel->y *= 0.7711f;
		lakituModel->y += 52.0f;
		lakituModel->scaleFactor = 0.77f;
	}

	static const float itemPos[ITEM_COUNT][2] = {
		{357.0f, 276.0f},
		{450.0f, 276.0f},
		{543.0f, 276.0f},
		{636.0f, 276.0f},

		{380.0f, 190.0f},
		{462.0f, 190.0f},
		{544.0f, 190.0f},

		{363.0f, 104.0f},
		{413.0f, 104.0f},
		{463.0f, 104.0f},
		{513.0f, 104.0f},
		{563.0f, 104.0f},
	};

	itemModels = new ShopModel_c[ITEM_COUNT];
	for (int i = 0; i < ITEM_COUNT; i++) {
		float effectiveX = itemPos[i][0];
		float effectiveY = itemPos[i][1];
		if (!IsWideScreen()) {
			effectiveX = (0.731f * (effectiveX + 416.0f)) - 296.0f;
			effectiveY *= 0.7711f;
			effectiveY += 50.0f;
			itemModels[i].scaleFactor = 1.6f;
		}
		itemModels[i].setupItem(effectiveX, effectiveY, Inventory[shopKind][i]);
	}
}
void dWMShop_c::deleteModels() {
	if (lakituModel)
		delete lakituModel;
	lakituModel = 0;

	if (itemModels)
		delete[] itemModels;
	itemModels = 0;
}


void dWMShop_c::loadInfo() {
	SaveBlock *save = GetSaveFile()->GetBlock(-1);

	leftCol.colourise(save->hudHintH, save->hudHintS, save->hudHintL);
	midCol.colourise(save->hudHintH, save->hudHintS, save->hudHintL);
	rightCol.colourise(save->hudHintH, save->hudHintS, save->hudHintL);

	// find out the shop name
	dLevelInfo_c::entry_s *shopNameEntry =
		dLevelInfo_c::s_info.searchBySlot(shopKind, 98);

	wchar_t shopName[100];
	// TODO: refactor this a bit
	const char *sourceName = dLevelInfo_c::s_info.getNameForLevel(shopNameEntry);
	int charCount = 0;
	
	while (*sourceName != 0 && charCount < 99) {
		shopName[charCount] = *sourceName;
		sourceName++;
		charCount++;
	}
	shopName[charCount] = 0;

	Title->SetString(shopName);
	TitleShadow->SetString(shopName);

	// load the coin count
	int scCount = getUnspentStarCoinCount();
	WriteNumberToTextBox(&scCount, CoinCount, false);
	WriteNumberToTextBox(&scCount, CoinCountShadow, false);


	WriteBMGToTextBox(BackText, GetBMG(), 2, 58, 0);
	WriteBMGToTextBox(BuyText, GetBMG(), 302, 4, 0);
}


void dWMShop_c::buyItem(int item) {
	static int itemDefs[6][3] = {
		// Cost, Start Index, Count
		{1, 0, 1}, {2, 1, 1}, {2, 2, 1}, {3, 3, 1},
		{5, 4, 3}, {8, 7, 5}
	};

	int cost = itemDefs[item][0], cash = getUnspentStarCoinCount();

	if (cost > cash) {
		OSReport("Started playing Not Enough\n");
		lakituModel->playingNotEnough = true;
		lakituModel->playAnim("notenough", 1.0f, 1);
		MapSoundPlayer(SoundRelatedClass, SE_SYS_INVALID, 1);
		return;
	}

	MapSoundPlayer(SoundRelatedClass, SE_SYS_DECIDE, 1);

	SaveFile *file = GetSaveFile();
	SaveBlock *block = file->GetBlock(file->header.current_file);

	coinsRemaining = cost;

	// Work out what we need to apply
	int appliedItems[ITEM_TYPE_COUNT];
	for (int i = 0; i < ITEM_TYPE_COUNT; i++)
		appliedItems[i] = 0;

	int invStartIndex = itemDefs[item][1], invCount = itemDefs[item][2];
	for (int i = 0; i < invCount; i++)
		appliedItems[(int)Inventory[shopKind][invStartIndex+i]]++;

	for (int i = 0; i < 8; i++) {
		block->powerups_available[i] += appliedItems[i];

		if (block->powerups_available[i] > 99)
			block->powerups_available[i] = 99;

		dScKoopatlas_c::instance->stockItem->newCounts[i] = block->powerups_available[i];
	}

	// Apply lives to everyone
	for (int i = 0; i < 4; i++) {
		if (Player_Active[i]) {
			int id = Player_ID[i];
			Player_Lives[id] += appliedItems[(int)ONE_UP];
			if (Player_Lives[id] > 99)
				Player_Lives[id] = 99;
		}
	}

	if (appliedItems[(int)ONE_UP] > 0)
		MapSoundPlayer(SoundRelatedClass, SE_SYS_100COIN_ONE_UP, 1);

	state.setState(&StateID_CoinCountdown);
	HideSelectCursor(SelectCursorPointer, 0);
}


void dWMShop_c::beginState_CoinCountdown() {
	timerForCoinCountdown = 8;
}

void dWMShop_c::endState_CoinCountdown() { }

void dWMShop_c::executeState_CoinCountdown() {
	timerForCoinCountdown--;
	if (timerForCoinCountdown <= 0) {

		SaveBlock *save = GetSaveFile()->GetBlock(-1);
		save->spentStarCoins++;

		// load the coin count
		int scCount = getUnspentStarCoinCount();
		WriteNumberToTextBox(&scCount, CoinCount, false);
		WriteNumberToTextBox(&scCount, CoinCountShadow, false);

		layout.enableNonLoopAnim(COUNT_COIN);
		VEC3 efPos = {
			CoinCount->effectiveMtx[0][3],
			CoinCount->effectiveMtx[1][3],
			0.0f};

		// ARGHHHHHHHHHHHHHHHHh.
		if (IsWideScreen()) {
			float div = 5.0f;
			if (scCount < 100)
				div = 3.6f;
			if (scCount < 10)
				div = 2.7f;
			efPos.x -= (CoinCount->size.x / div);
			efPos.y -= (CoinCount->size.y / 2.0f);
		} else {
			float div = 5.8f;
			if (scCount < 100)
				div = 8.2f;
			if (scCount < 10)
				div = 14.5f;
			efPos.x += (CoinCount->size.x / div);
			efPos.y -= (CoinCount->size.y / 2.8f);
		}

		VEC3 efScale = {0.7f, 0.7f, 0.7f};
		SpawnEffect("Wm_2d_moviecoinvanish", 0, &efPos, 0, &efScale);

		coinsRemaining--;
		if (coinsRemaining <= 0) {
			MapSoundPlayer(SoundRelatedClass, SE_PLY_GET_ITEM_AGAIN, 1);
			state.setState(&StateID_Wait);
		} else {
			MapSoundPlayer(SoundRelatedClass, SE_SYS_STAR_COIN_PAY, 1);
			beginState_CoinCountdown();
		}
	}
}


void dWMShop_c::showSelectCursor() {
	switch (selected) {
		case 4: UpdateSelectCursor(Btn1Base, 0, false); break;
		case 5: UpdateSelectCursor(Btn2Base, 0, false); break;
		default: UpdateSelectCursor(Buttons[selected], 0, false);
	}
}


//
// processed\../src/koopatlas/starcoin.cpp
//

#include "koopatlas/starcoin.h"
#include <game.h>

dWMStarCoin_c *dWMStarCoin_c::instance = 0;

dWMStarCoin_c *dWMStarCoin_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(dWMStarCoin_c));
	dWMStarCoin_c *c = new(buffer) dWMStarCoin_c;

	instance = c;
	return c;
}

dWMStarCoin_c::dWMStarCoin_c() : state(this) {
	layoutLoaded = false;
	visible = false;
	state.setState(&StateID_Hidden);
}

CREATE_STATE(dWMStarCoin_c, Hidden);
CREATE_STATE(dWMStarCoin_c, ShowWait);
CREATE_STATE(dWMStarCoin_c, ShowSectionWait);
CREATE_STATE(dWMStarCoin_c, Wait);
CREATE_STATE(dWMStarCoin_c, HideSectionWait);
CREATE_STATE(dWMStarCoin_c, HideWait);

int dWMStarCoin_c::onCreate() {

	if (!layoutLoaded) {
		bool gotFile = layout.loadArc("StarCoins.arc", false);
		if (!gotFile)
			return false;

		bool output = layout.build("StarCoins.brlyt");

		layout.layout.rootPane->trans.x = -112.0f;
		if (IsWideScreen()) {
			layout.layout.rootPane->scale.x = 0.735f;
		} else {
			layout.clippingEnabled = true;
			layout.clipX = 0;
			layout.clipY = 52;
			layout.clipWidth = 640;
			layout.clipHeight = 352;
			layout.layout.rootPane->scale.x = 0.731f;
			layout.layout.rootPane->scale.y = 0.7711f;
		}

		static const char *brlanNames[] = {
			"StarCoins_Show.brlan",
			"StarCoins_ShowSection.brlan",
			"StarCoins_HideSection.brlan",
			"StarCoins_ShowArrow.brlan",
			"StarCoins_HideArrow.brlan",
		};
		static const char *groupNames[] = {
			"base", "section", "section", "leftArrow", "leftArrow", "rightArrow", "rightArrow"
		};

		layout.loadAnimations(brlanNames, 5);
		layout.loadGroups(groupNames, (int[7]){0, 1, 2, 3, 4, 3, 4}, 7);
		layout.disableAllAnimations();

		layout.drawOrder = 1;

		for (int col = 0; col < COLUMN_COUNT; col++) {
			for (int shine = 0; shine < SHINE_COUNT; shine++) {
				char name[8];
				sprintf(name, "Shine%d%d", col, shine);
				Shine[col][shine] = layout.findPictureByName(name);
			}

			for (int row = 0; row < ROW_COUNT; row++) {
				char lname[12];
				sprintf(lname, "LevelName%d%d", col, row);
				LevelName[col][row] = layout.findTextBoxByName(lname);

				char coname[16], cname[8];
				for (int i = 0; i < 3; i++) {
					sprintf(coname, "CoinOutline%d%d%d", col, row, i);
					CoinOutline[col][row][i] = layout.findPictureByName(coname);

					sprintf(cname, "Coin%d%d%d", col, row, i);
					Coin[col][row][i] = layout.findPictureByName(cname);
				}
			}
		}

		static const char *tbNames[] = {
			"LeftTitle", "RightTitle", "TotalCoinCount", "UnspentCoinCount",
			"EarnedCoinCount", "EarnedCoinMax", "BtnBackText",
		};
		layout.getTextBoxes(tbNames, &LeftTitle, 7);

		static const char *picNames[] = {
			"DPadLeft", "DPadRight",
		};
		layout.getPictures(picNames, &DPadLeft, 2);

		DPadLeft->SetVisible(false);
		DPadRight->SetVisible(false);

		layoutLoaded = true;
	}

	return true;
}


int dWMStarCoin_c::onDelete() {
	return layout.free();
}


void dWMStarCoin_c::show() {
	if (state.getCurrentState() == &StateID_Hidden)
		state.setState(&StateID_ShowWait);
}


int dWMStarCoin_c::onExecute() {
	state.execute();

	if (visible) {
		layout.execAnimations();
		layout.update();
	}

	return true;
}

int dWMStarCoin_c::onDraw() {
	if (visible)
		layout.scheduleForDrawing();

	return true;
}


void dWMStarCoin_c::showLeftArrow() {
	if (!isLeftArrowVisible) {
		isLeftArrowVisible = true;
		layout.enableNonLoopAnim(SHOW_LEFT_ARROW);
		DPadLeft->SetVisible(true);
	}
}

void dWMStarCoin_c::showRightArrow() {
	if (!isRightArrowVisible) {
		isRightArrowVisible = true;
		layout.enableNonLoopAnim(SHOW_RIGHT_ARROW);
		DPadRight->SetVisible(true);
	}
}

void dWMStarCoin_c::hideLeftArrow() {
	if (isLeftArrowVisible) {
		isLeftArrowVisible = false;
		layout.enableNonLoopAnim(HIDE_LEFT_ARROW);
		DPadLeft->SetVisible(false);
	}
}

void dWMStarCoin_c::hideRightArrow() {
	if (isRightArrowVisible) {
		isRightArrowVisible = false;
		layout.enableNonLoopAnim(HIDE_RIGHT_ARROW);
		DPadRight->SetVisible(false);
	}
}

void dWMStarCoin_c::setLeftArrowVisible(bool value) {
	if (value)
		showLeftArrow();
	else
		hideLeftArrow();
}

void dWMStarCoin_c::setRightArrowVisible(bool value) {
	if (value)
		showRightArrow();
	else
		hideRightArrow();
}


bool dWMStarCoin_c::canScrollLeft() const {
	return (currentSectionIndex > 0);
}
bool dWMStarCoin_c::canScrollRight() const {
	return (currentSectionIndex < (availableSectionCount - 1));
}

void dWMStarCoin_c::loadInfo() {
	WriteBMGToTextBox(BtnBackText, GetBMG(), 3, 1, 0);

	int unspentCoins = getUnspentStarCoinCount();
	int coins = getStarCoinCount();

	WriteNumberToTextBox(&unspentCoins, UnspentCoinCount, false);
	WriteNumberToTextBox(&coins, TotalCoinCount, false);

	currentSection = -1;
	currentSectionIndex = -1;
	availableSectionCount = 0;

	SaveBlock *save = GetSaveFile()->GetBlock(-1);
	int wantedSection = save->newerWorldID;

	// figure out which sections should be available
	for (int i = 0; i < dLevelInfo_c::s_info.sectionCount(); i++) {
		dLevelInfo_c::section_s *section = dLevelInfo_c::s_info.getSectionByIndex(i);

		bool haveLevels = false;
		for (int j = 0; j < section->levelCount; j++) {
			dLevelInfo_c::entry_s *l = &section->levels[j];
			if (l->flags & 2) {
				if (save->GetLevelCondition(l->worldSlot, l->levelSlot) & COND_UNLOCKED) {
					haveLevels = true;
					break;
				}
			}
		}

		if (haveLevels) {
			if (i == wantedSection) {
				currentSection = wantedSection;
				currentSectionIndex = availableSectionCount;
			}
			sectionIndices[availableSectionCount++] = i;
		}
	}

	// if we didn't find the wanted one, use the first one available
	if (currentSectionIndex == -1) {
		currentSectionIndex = 0;
		currentSection = sectionIndices[0];
	}
}

void dWMStarCoin_c::loadSectionInfo() {
	dLevelInfo_c::entry_s *visibleLevels[COLUMN_COUNT][ROW_COUNT];

	// reset everything... everything
	for (int i = 0; i < COLUMN_COUNT; i++) {
		for (int j = 0; j < SHINE_COUNT; j++)
			Shine[i][j]->SetVisible(false);

		for (int j = 0; j < ROW_COUNT; j++) {
			visibleLevels[i][j] = 0;

			LevelName[i][j]->SetVisible(false);

			for (int k = 0; k < 3; k++) {
				CoinOutline[i][j][k]->SetVisible(false);
				Coin[i][j][k]->SetVisible(false);
			}
		}
	}

	// get everything we'll need
	SaveBlock *save = GetSaveFile()->GetBlock(-1);
	dLevelInfo_c *linfo = &dLevelInfo_c::s_info;

	dLevelInfo_c::entry_s *names[COLUMN_COUNT];
	for (int i = 0; i < COLUMN_COUNT; i++)
		names[i] = linfo->searchByDisplayNum(currentSection, 100+i);

	bool useSubworlds = (COLUMN_COUNT > 1) && names[1];

	int currentPosition[COLUMN_COUNT];
	int currentColumn = 0; // only incremented in single-subworld mode

	for (int i = 0; i < COLUMN_COUNT; i++)
		currentPosition[i] = 0;

	dLevelInfo_c::section_s *section = linfo->getSectionByIndex(currentSection);

	int earnedCoins = 0, earnableCoins = 0;
	// earnedCoins is calculated later

	for (int i = 0; i < section->levelCount; i++) {
		dLevelInfo_c::entry_s *level = &section->levels[i];

		// only pay attention to real levels
		if (!(level->flags & 2))
			continue;

		earnableCoins += 3;

		// is this level unlocked?
		u32 conds = save->GetLevelCondition(level->worldSlot, level->levelSlot);

		if (!(conds & COND_UNLOCKED))
			continue;

		// well, let's give it a slot
		if (useSubworlds) {
			currentColumn = (level->flags & 0x400) ? 1 : 0;
		} else {
			if (currentPosition[currentColumn] >= ROW_COUNT)
				currentColumn++;
		}

		visibleLevels[currentColumn][currentPosition[currentColumn]++] = level;
	}

	// if the first column is empty, then move the second one over
	if (currentPosition[0] == 0 && useSubworlds) {
		for (int i = 0; i < currentPosition[1]; i++) {
			visibleLevels[0][i] = visibleLevels[1][i];
			visibleLevels[1][i] = 0;
		}

		names[0] = names[1];
		names[1] = 0;
	}

	// if the second column is empty, remove its name
	if (currentPosition[1] == 0 && useSubworlds)
		names[1] = 0;

	// work out the names
	WriteAsciiToTextBox(LeftTitle, linfo->getNameForLevel(names[0]));
#ifdef FALLING_LEAF
	LeftTitle->SetVisible(false);
#endif
	if (names[1])
		WriteAsciiToTextBox(RightTitle, linfo->getNameForLevel(names[1]));
	RightTitle->SetVisible(names[1] != 0);

	// load all level info
	for (int col = 0; col < COLUMN_COUNT; col++) {
		for (int row = 0; row < ROW_COUNT; row++) {
			dLevelInfo_c::entry_s *level = visibleLevels[col][row];
			if (!level)
				continue;

			u32 conds = save->GetLevelCondition(level->worldSlot, level->levelSlot);

			if (!(row & 1)) {
				int shineID = row / 2;
				if (shineID < SHINE_COUNT)
					Shine[col][shineID]->SetVisible(true);
			}

			for (int coin = 0; coin < 3; coin++) {
				CoinOutline[col][row][coin]->SetVisible(true);

				if (conds & (COND_COIN1 << coin)) {
					Coin[col][row][coin]->SetVisible(true);
					earnedCoins++;
				}
			}

			LevelName[col][row]->SetVisible(true);
			WriteAsciiToTextBox(LevelName[col][row], linfo->getNameForLevel(level));
		}
	}

	// set up coin things
	WriteNumberToTextBox(&earnedCoins, EarnedCoinCount, false);
	WriteNumberToTextBox(&earnableCoins, EarnedCoinMax, false);
}


void dWMStarCoin_c::beginState_Hidden() { }
void dWMStarCoin_c::executeState_Hidden() { }
void dWMStarCoin_c::endState_Hidden() { }

static const int secretCode[] = {
	WPAD_UP,WPAD_UP,WPAD_DOWN,WPAD_DOWN,
	WPAD_LEFT,WPAD_RIGHT,WPAD_LEFT,WPAD_RIGHT,
	WPAD_ONE,WPAD_TWO,0
};
static const int secretCodeButtons = WPAD_UP|WPAD_DOWN|WPAD_LEFT|WPAD_RIGHT|WPAD_ONE|WPAD_TWO;
static int secretCodeIndex = 0;
static int minusCount = 0;
extern bool enableHardMode;
extern bool enableDebugMode;
extern u8 isReplayEnabled;

void dWMStarCoin_c::beginState_ShowWait() {
	visible = true;
	loadInfo();
	layout.enableNonLoopAnim(SHOW_ALL);
	layout.resetAnim(SHOW_SECTION);
	layout.resetAnim(SHOW_LEFT_ARROW);
	layout.resetAnim(SHOW_RIGHT_ARROW);

	secretCodeIndex = 0;
	minusCount = 0;
}
void dWMStarCoin_c::executeState_ShowWait() {
	if (!layout.isAnimOn(SHOW_ALL))
		state.setState(&StateID_ShowSectionWait);
}
void dWMStarCoin_c::endState_ShowWait() { }

void dWMStarCoin_c::beginState_ShowSectionWait() {
	loadSectionInfo();
	layout.enableNonLoopAnim(SHOW_SECTION);

	if (canScrollLeft())
		showLeftArrow();
	if (canScrollRight())
		showRightArrow();
}
void dWMStarCoin_c::executeState_ShowSectionWait() {
	if (!layout.isAnimOn(SHOW_SECTION))
		state.setState(&StateID_Wait);
}
void dWMStarCoin_c::endState_ShowSectionWait() { }

void dWMStarCoin_c::showSecretMessage(const wchar_t *title, const wchar_t **body, int lineCount, const wchar_t **body2, int lineCount2) {
	LeftTitle->SetVisible(true);
	LeftTitle->SetString(title);
	RightTitle->SetVisible(false);

	for (int c = 0; c < COLUMN_COUNT; c++) {
		for (int i = 0; i < SHINE_COUNT; i++)
			Shine[c][i]->SetVisible(false);
		for (int r = 0; r < ROW_COUNT; r++) {
			LevelName[c][r]->SetVisible(false);
			for (int i = 0; i < 3; i++) {
				CoinOutline[c][r][i]->SetVisible(false);
				Coin[c][r][i]->SetVisible(false);
			}
		}
	}

	for (int i = 0; i < lineCount; i++) {
		LevelName[0][i]->SetVisible(true);
		LevelName[0][i]->SetString(body[i]);
	}

	if (body2) {
		for (int i = 0; i < lineCount2; i++) {
			LevelName[1][i]->SetVisible(true);
			LevelName[1][i]->SetString(body2[i]);
		}
	}
}

void dWMStarCoin_c::beginState_Wait() { }
void dWMStarCoin_c::executeState_Wait() {
	int nowPressed = Remocon_GetPressed(GetActiveRemocon());

	// A and Plus (formerly B as well, prior to 1.30, but nunchuk scheme binds B to cancel)
	if ((GetActiveRemocon()->heldButtons == 0x810) && (nowPressed & 0x810)) {

		const int lineCountOn = 9, lineCountOff = 2;
		static const wchar_t *linesOn[lineCountOn] = {
			L"You've activated Hard Mode!",
			L" ",
			L"In Hard Mode, Mario will die",
			L"any time he takes damage, and",
			L"the timer will be more strict.",
			L" ",
			L"So treasure your Yoshi and",
			L"hold on to your hat, 'cause",
			L"you're in for a wild ride!",
		};
		static const wchar_t *linesOff[lineCountOff] = {
			L"Hard Mode has been",
			L"turned off.",
		};

		if (!enableHardMode) {
			enableHardMode = true;
			OSReport("Hard Mode enabled!\n");
			MapSoundPlayer(SoundRelatedClass, SE_VOC_MA_CS_COURSE_IN_HARD, 1);
			showSecretMessage(L"Hard Mode", linesOn, lineCountOn);
		} else {
			enableHardMode = false;
			OSReport("Hard Mode disabled!\n");
			showSecretMessage(L"Classic Mario", linesOff, lineCountOff);
		}
		return;
	}

	if (nowPressed & secretCodeButtons) {
		int nextKey = secretCode[secretCodeIndex];
		if (nowPressed & nextKey) {
			secretCodeIndex++;
			if (secretCode[secretCodeIndex] == 0) {
				secretCodeIndex = 0;
				MapSoundPlayer(SoundRelatedClass, SE_VOC_MA_THANK_YOU, 1);
				//enableDebugMode = !enableDebugMode;
				//OSReport("Debug mode toggled!\n");
				const int lineCountOn = 9, lineCountOff = 2;
				static const wchar_t *linesOn[lineCountOn] = {
					L"The experimental Replay",
					L"Recording feature has",
					L"been enabled. Enjoy!",
					L"You'll find your Replays",
					L"on your SD or USB, depending",
					L"on where Newer's files are.",
					L"It might not work, so",
					L"save your game before you",
					L"play a level!",
				};
				static const wchar_t *linesOff[lineCountOff] = {
					L"Replay Recording",
					L"turned off.",
				};

				if (isReplayEnabled != 100) {
					isReplayEnabled = 100;
					OSReport("Replay Recording enabled!\n");
					showSecretMessage(L"Nice!", linesOn, lineCountOn);
				} else {
					isReplayEnabled = 0;
					OSReport("Replay Recording disabled!\n");
					showSecretMessage(L"Nice!", linesOff, lineCountOff);
				}
			}
			return;
		} else {
			secretCodeIndex = 0;
		}
	}

	if (nowPressed & WPAD_MINUS) {
		minusCount++;
		if (minusCount >= 16) {
			minusCount = 0;

			enableDebugMode = !enableDebugMode;

			if (enableDebugMode) {
				MapSoundPlayer(SoundRelatedClass, SE_VOC_MA_GET_PRIZE, 1);

				const int msgCount = 9;
				static const wchar_t *msg[msgCount] = {
					L"You've found the Totally",
					L"Secret Collision Debug Mode.",
					L"We used this to make the",
					L"hitboxes on our custom sprites",
					L"and bosses suck less. Awesome,",
					L"right?!",
					L"Actually, I did it just to waste",
					L"some time, but it ended up",
					L"being pretty useful!",
				};
				const int msgCount2 = 9;
				static const wchar_t *msg2[msgCount2] = {
					L"And yes, I know it doesn't show",
					L"a couple of things properly",
					L"like round objects and rolling",
					L"hills and so on.",
					L"Can't have it all, can you?",
					L"Wonder if Nintendo had",
					L"something like this...",
					L"",
					L"    Treeki, 9th February 2013",
				};
				showSecretMessage(L"Groovy!", msg, msgCount, msg2, msgCount2);
			} else {
				const int msgCount = 6;
				static const wchar_t *msg[msgCount] = {
					L"You've turned off the Totally",
					L"Secret Collision Debug Mode.",
					L"",
					L"... and no, I'm not going to write",
					L"another ridiculously long",
					L"message to go here. Sorry!",
				};
				static const wchar_t *hiddenMsg[] = {
					L"If you found these messages by",
					L"looking through strings in the DLCode",
					L"file, then... that's kind of cheating.",
					L"Though I can't say I wouldn't do the",
					L"same!",
					L"You won't actually see this in game",
					L"btw :p So why am I bothering with linebreaks anyway? I dunno. Oh well.",
					L"Also, don't put this message on TCRF. Or do! Whatever. :(",
				};
				showSecretMessage(L"Groovy!", msg, msgCount, hiddenMsg, 0);
			}
		}
	} else if (nowPressed & WPAD_ONE) {
		MapSoundPlayer(SoundRelatedClass, SE_SYS_DIALOGUE_OUT_AUTO, 1);
		willExit = true;
		state.setState(&StateID_HideSectionWait);
	} else if ((nowPressed & WPAD_LEFT) && canScrollLeft()) {
		currentSection = sectionIndices[--currentSectionIndex];
		willExit = false;
		state.setState(&StateID_HideSectionWait);
	} else if ((nowPressed & WPAD_RIGHT) && canScrollRight()) {
		currentSection = sectionIndices[++currentSectionIndex];
		willExit = false;
		state.setState(&StateID_HideSectionWait);
	}
}
void dWMStarCoin_c::endState_Wait() { }

void dWMStarCoin_c::beginState_HideSectionWait() {
	layout.enableNonLoopAnim(HIDE_SECTION);
	if (willExit) {
		hideLeftArrow();
		hideRightArrow();
	} else {
		setLeftArrowVisible(canScrollLeft());
		setRightArrowVisible(canScrollRight());
	}
}
void dWMStarCoin_c::executeState_HideSectionWait() {
	if (!layout.isAnimOn(HIDE_SECTION)) {
		if (willExit)
			state.setState(&StateID_HideWait);
		else
			state.setState(&StateID_ShowSectionWait);
	}
}
void dWMStarCoin_c::endState_HideSectionWait() { }

void dWMStarCoin_c::beginState_HideWait() {
	layout.enableNonLoopAnim(SHOW_ALL, true);
	layout.grpHandlers[SHOW_ALL].frameCtrl.flags = 3; // NO_LOOP | REVERSE
}
void dWMStarCoin_c::executeState_HideWait() {
	if (!layout.isAnimOn(SHOW_ALL))
		state.setState(&StateID_Hidden);
}
void dWMStarCoin_c::endState_HideWait() {
	visible = false;
}


//
// processed\../src/koopatlas/mapmusic.cpp
//

#include <game.h>
#include "koopatlas/mapmusic.h"
#include "music.h"

extern "C" void PlaySoundWithFunctionB4(void *spc, nw4r::snd::SoundHandle *handle, int id, int unk);

static nw4r::snd::StrmSoundHandle s_handle;
static bool s_playing = false;

static nw4r::snd::SoundHandle s_starHandle;
static bool s_starPlaying = false;

static int s_song = -1;
static int s_nextSong = -1;

static int s_countdownToSwitch = -1;
static int s_countdownToFadeIn = -1;

static dDvdLoader_c s_adpcmInfoLoader;

#define FADE_OUT_LEN 30
#define FADE_IN_LEN 30
#define BUFFER_CLEAR_DELAY 60

u8 hijackMusicWithSongName(const char *songName, int themeID, bool hasFast, int channelCount, int trackCount, int *wantRealStreamID);

void dKPMusic::play(int id) {
	if (s_playing) {
		// Switch track
		OSReport("Trying to switch to song %d (Current one is %d)...\n", id, s_song);
		if ((s_song == id && s_nextSong == -1) || s_nextSong == id) {
			OSReport("This song is already playing or is scheduled. Not gonna do that.\n");
		//} else if (s_countdownToFadeIn >= 0) {
		//	OSReport("There's already a track being faded in (CountdownToFadeIn=%d)\n", s_countdownToFadeIn);
		} else if (s_countdownToSwitch >= 0) {
			OSReport("We were already going to switch tracks, but the rstm hasn't been changed yet, so the next song is being changed to this one\n");
			s_nextSong = id;
		} else {
			OSReport("Will switch; Fading out current track 2 over %d frames\n", FADE_OUT_LEN);

			if (s_handle.Exists())
				s_handle.SetTrackVolume(1<<1, FADE_OUT_LEN, 0.0f);
			s_nextSong = id;
			s_countdownToSwitch = FADE_OUT_LEN;
		}

	} else {
		// New track
		OSReport("Playing song %d from the start.\n", id);

		int realStreamID;
		char brstmName[8];
		sprintf(brstmName, "map%d", id);
		hijackMusicWithSongName(brstmName, -1, false, 4, 2, &realStreamID);

		PlaySoundWithFunctionB4(SoundRelatedClass, &s_handle, realStreamID, 1);

		s_playing = true;
		s_song = id;
	}
}

// crap.
#include "fileload.h"

extern "C" void DVDCancel(void *crap);

extern "C" void AxVoice_SetADPCM(void *axVoice, void *adpcm);
extern "C" void Voice_SetADPCMLoop(void *voice, int channel, void *adpcmLoop);

bool dKPMusic::loadInfo() {
	return s_adpcmInfoLoader.load("/NewerRes/MapADPCMInfo.bin");
}

void dKPMusic::execute() {
	if (!s_playing)
		return;

	if (s_handle.GetSound() == 0) {
		nw4r::db::Exception_Printf_("SOUND IS NOT PLAYING!\n");
		return;
	}

	if (s_countdownToSwitch >= 0) {
		s_countdownToSwitch--;
		if (s_countdownToSwitch == 0) {
			nw4r::db::Exception_Printf_("Switching brstm files to song %d.\n", s_nextSong);

			char brstmPath[48];
			sprintf(brstmPath, "/Sound/new/map%d.er", s_nextSong);

			u8 *sound = (u8*)(s_handle.GetSound());
			u8 *player = sound+0x110;
			u8 **fileStreamPointer = (u8**)(player+0x808);
			u8 *fileStream = *fileStreamPointer;
			DVDHandle *fileInfo = (DVDHandle*)(fileStream+0x28);

			if (fileInfo->unk4 == 1) {
				OSReport("Was reading chunk, will try again next frame...\n");
				s_countdownToSwitch++;
				return;
			}

			if (s_nextSong > 0)
				s_countdownToFadeIn = BUFFER_CLEAR_DELAY;

			DVDCancel(fileInfo);
			//OSReport("CANCEL successfully called!\n");
			bool result = DVDFastOpen(DVDConvertPathToEntrynum(brstmPath), fileInfo);

			nw4r::db::Exception_Printf_("StrmSound is at %p, StrmPlayer is at %p, FileStream pointer is at %p, FileStream is at %p, FileInfo is at %p\n", sound, player, fileStreamPointer, fileStream, fileInfo);
			nw4r::db::Exception_Printf_("Changed to name %s. FastOpen returned: %d\n", brstmPath, result);

			u8 *trackArray = player+0xB58;
			u8 *track = (trackArray + (0x38 * 1));
			u8 **voicePointer = (u8**)(track+4);
			u8 *voice = *voicePointer;
			nw4r::db::Exception_Printf_("Track Array: %p; Track: %p; Voice Pointer: %p; Voice: %p\n", trackArray, track, voicePointer, voice);

			for (int i = 0; i < 2; i++) {
				int sourceBlockID = (s_nextSong*2)+i;
				u8 *sourceData = ((u8*)(s_adpcmInfoLoader.buffer)) + (0x30*sourceBlockID);
				nw4r::db::Exception_Printf_("Using ADPCM data for channel %d from block %d, data at %p\n", i, sourceBlockID, sourceData);

				if (!voice)
					continue;

				Voice_SetADPCMLoop(voice, i, sourceData+0x28);

				// loop through all axVoices
				for (int j = 0; j < 4; j++) {
					int axVoiceID = (i*4) + j;
					u8 **axVoicePointer = (u8**)(voice + 0xC + (axVoiceID*4));
					u8 *axVoice = *axVoicePointer;
					nw4r::db::Exception_Printf_("Setting AxVoice ID %d, with pointer at %p, located at %p\n", axVoiceID, axVoicePointer, axVoice);

					if (axVoice)
						AxVoice_SetADPCM(axVoice, sourceData);
				}
			}

			OSReport("All done\n");

			s_song = s_nextSong;
			s_nextSong = -1;
		}

	} else if (s_countdownToFadeIn >= 0) {
		s_countdownToFadeIn--;
		if (s_countdownToFadeIn == 0) {
			OSReport("Going to fade in the second track now!\n");
			if (s_handle.Exists())
				s_handle.SetTrackVolume(1<<1, FADE_IN_LEN, 1.0f);
		}
	}
}

void dKPMusic::stop() {
	if (!s_playing)
		return;

	OSReport("Stopping song\n");

	s_playing = false;
	s_song = -1;
	s_nextSong = -1;
	s_countdownToSwitch = -1;
	s_countdownToFadeIn = -1;

	if (s_handle.Exists())
		s_handle.Stop(30);

	if (s_starHandle.Exists())
		s_starHandle.Stop(15);
}


void dKPMusic::playStarMusic() {
	if (s_starPlaying)
		return;

	PlaySoundWithFunctionB4(SoundRelatedClass, &s_starHandle, SE_BGM_CS_STAR, 1);
	s_starPlaying = true;
}


//
// processed\../src/texmapcolouriser.cpp
//

#include "texmapcolouriser.h"

dTexMapColouriser_c::dTexMapColouriser_c() {
	texmap = 0;
	original = 0;
	mine = 0;
}

void *EGG__Heap__alloc(unsigned long size, int unk, void *heap);
void EGG__Heap__free(void *ptr, void *heap);

dTexMapColouriser_c::~dTexMapColouriser_c() {
	resetAndClear();
}

void dTexMapColouriser_c::resetAndClear() {
	texmap = 0;
	if (mine) {
		EGG__Heap__free(mine, 0);
		mine = 0;
	}
}

void dTexMapColouriser_c::setTexMap(nw4r::lyt::TexMap *tm) {
	OSReport("Colourising TexMap: %p (w:%d h:%d)\n", tm, tm->width, tm->height);
	if (texmap)
		resetAndClear();

	if (tm->getFormat() != GX_TF_IA8) {
		OSReport("Warning: Trying to colourise image whose format is %d not GX_TF_IA8\n", tm->getFormat());
	}

	texmap = tm;
	original = (u16*)tm->image;
	mine = (u16*)EGG__Heap__alloc(tm->width * tm->height * 4, 0x20, mHeap::gameHeaps[2]);
	tm->image = mine;
	tm->setFormat(GX_TF_RGBA8);
}

void dTexMapColouriser_c::applyAlso(nw4r::lyt::TexMap *tm) {
	if (!texmap) {
		setTexMap(tm);
	} else {
		tm->image = mine;
		tm->setFormat(GX_TF_RGBA8);
	}
}

inline static float hslValue(float n1, float n2, float hue) {
	if (hue > 6.0f)
		hue -= 6.0f;
	else if (hue < 0.0f)
		hue += 6.0f;

	if (hue < 1.0f)
		return n1 + (n2 - n1) * hue;
	else if (hue < 3.0f)
		return n2;
	else if (hue < 4.0f)
		return n1 + (n2 - n1) * (4.0f - hue);
	else
		return n1;
}

void dTexMapColouriser_c::colourise(int h, int s, int l) {
	if (!mine)
		return;

	int width = texmap->width, height = texmap->height;
	int texelW = width / 4, texelH = height / 4;

	u16 *source = original, *dest = mine;

	float hueParam = h / 360.0f;
	float satParam = s / 100.0f;
	float lumParam = l / 100.0f;

	for (int texelY = 0; texelY < texelH; texelY++) {
		for (int texelX = 0; texelX < texelW; texelX++) {
			for (int y = 0; y < 4; y++) {
				for (int x = 0; x < 4; x++) {
					u8 intensity = *source & 0xFF;
					u8 alpha = *source >> 8;

					u8 r, g, b;

					// This is a hack
					if (alpha < 250) {
						r = g = b = intensity;
					} else {
						// converting from GIMP's colourise code...
						// h and s are always the same
						// l is the only thing we need to touch:
						// we get the luminance from the source pixel
						// (which, conveniently, is the intensity)
						// manipulate it using the passed l and then
						// convert the whole thing to RGB

						float lum = intensity / 255.0f;

						// manipulate it
						if (l > 0) {
							lum = lum * (1.0f - lumParam);
							lum += (1.0f - (1.0f - lumParam));
						} else if (l < 0) {
							lum = lum * (lumParam + 1.0f);
						}

						// make it RGB

						if (s == 0) {
							r = g = b = lum*255.0f;
						} else {
							float m1, m2;
							if (lum <= 0.5f)
								m2 = lum * (1.0f + satParam);
							else
								m2 = lum + satParam - lum * satParam;

							m1 = 2.0f * lum - m2;

							r = hslValue(m1, m2, hueParam * 6.0f + 2.0) * 255.0f;
							g = hslValue(m1, m2, hueParam * 6.0f) * 255.0f;
							b = hslValue(m1, m2, hueParam * 6.0f - 2.0) * 255.0f;
						}
					}

					// now write it
					dest[0] = (alpha<<8)|r;
					dest[16] = (g<<8)|b;

					source++;
					dest++;
				}
			}

			dest += 16;
		}
	}
}



//
// processed\../src/pregame.cpp
//

#include <game.h>
#include "levelinfo.h"
#include <newer.h>

class PregameLytHandler {
	public:
		m2d::EmbedLayout_c layout;

		nw4r::lyt::Pane *rootPane;

		nw4r::lyt::TextBox
			*T_minus_00, *T_world_00, *T_worldNum_00,
			*T_pictureFont_00, *T_corseNum_00,
			*T_remainder_00, *T_remainder_01, *T_remainder_02, *T_remainder_03,
			*T_remainder_10, *T_remainder_11, *T_remainder_12, *T_remainder_13,
			*T_x_00, *T_x_01, *T_x_02, *T_x_03, *T_x_10, *T_x_11, *T_x_12, *T_x_13,
			*T_x_00_o, *T_x_10_o,
			*T_otasukePlay_00, *T_otasukePlay_01,
			*T_recommend_00, *T_remainder_00_o, *T_remainder_10_o;

		nw4r::lyt::Picture
			*P_Wx_00[9], *P_coin_00, *P_free_00,
			*P_batB_0x[4], *P_bat_00,
			*P_batB_1x[4], *P_bat_01,
			*P_batB_2x[4], *P_bat_02,
			*P_batB_3x[4], *P_bat_03,
			*P_luijiIcon_00_o, *P_luijiIcon_10_o, *P_coinStage_00;

		nw4r::lyt::Pane
			*N_mario_00, *N_luiji_00, *N_kinoB_01, *N_kinoY_00,
			*N_zankiPos_x[4], *N_zanki_00,
			*Null_battPosxP[4], *N_batt_x[4],
			*N_batt, *N_otasukePlay_00;

		u8 layoutLoaded, somethingHasBeenDone, isVisible, hasShownLuigiThing_orSomething;

		u32 currentStateID;

		u32 _2E8;

		u32 countdownToEndabilityCopy, activePlayerCountMultBy4_maybe;
		u32 batteryLevels[4];
		u32 pgCountdown;

		void hijack_loadLevelNumber(); // replaces 80B6BDD0
};

// Notes:
// Deleted; P_coinStage_00, T_recommend_00, T_worldNum_00,
// T_-_00, T_pictureFont_00, T_corseNum_00, T_world_00
// P_Wx_00, P_coin_00, P_free_00

extern char CurrentLevel;
extern char CurrentWorld;

void LoadPregameStyleNameAndNumber(m2d::EmbedLayout_c *layout) {
	nw4r::lyt::TextBox
		*LevelNumShadow, *LevelNum,
		*LevelNameShadow, *LevelName;

	LevelNumShadow = layout->findTextBoxByName("LevelNumShadow");
	LevelNum = layout->findTextBoxByName("LevelNum");
	LevelNameShadow = layout->findTextBoxByName("LevelNameShadow");
	LevelName = layout->findTextBoxByName("LevelName");

	// work out the thing now
	dLevelInfo_c::entry_s *level = dLevelInfo_c::s_info.searchBySlot(CurrentWorld, CurrentLevel);
	if (level) {
		wchar_t convLevelName[160];
		const char *srcLevelName = dLevelInfo_c::s_info.getNameForLevel(level);
		int i = 0;
		while (i < 159 && srcLevelName[i]) {
			convLevelName[i] = srcLevelName[i];
			i++;
		}
		convLevelName[i] = 0;
		LevelNameShadow->SetString(convLevelName);
		LevelName->SetString(convLevelName);

		wchar_t levelNumber[32];
		wcscpy(levelNumber, L"Mundo ");
		getNewerLevelNumberString(level->displayWorld, level->displayLevel, &levelNumber[6]);

		LevelNum->SetString(levelNumber);

		// make the picture shadowy
		int sidx = 0;
		while (levelNumber[sidx]) {
			if (levelNumber[sidx] == 11) {
				levelNumber[sidx+1] = 0x200 | (levelNumber[sidx+1]&0xFF);
				sidx += 2;
			}
			sidx++;
		}
		LevelNumShadow->SetString(levelNumber);

	} else {
		LevelNameShadow->SetString(L"Not found in LevelInfo!");
		LevelName->SetString(L"Not found in LevelInfo!");
	}
}

#include "fileload.h"
void PregameLytHandler::hijack_loadLevelNumber() {
	LoadPregameStyleNameAndNumber(&layout);

	nw4r::lyt::Picture *LevelSample;
	LevelSample = layout.findPictureByName("LevelSample");

	// this is not the greatest way to read a file but I suppose it works in a pinch
	char tplName[64];
	sprintf(tplName, "/LevelSamples/%02d-%02d.tpl", CurrentWorld+1, CurrentLevel+1);
	static File tpl;
	if (tpl.open(tplName)) {
		LevelSample->material->texMaps[0].ReplaceImage((TPLPalette*)tpl.ptr(), 0);
	}
}





//
// processed\../src/layouthax.cpp
//

#include <game.h>

void LoadPregameStyleNameAndNumber(m2d::EmbedLayout_c *layout);
extern "C" void InsertPauseWindowText(void *thing) {
	m2d::EmbedLayout_c *el = (m2d::EmbedLayout_c*)(((u8*)thing)+0x70);
	LoadPregameStyleNameAndNumber(el);
}


//
// processed\../src/animtiles.cpp
//

#include <common.h>
#include <game.h>
#include "fileload.h"

struct AnimDef_Header {
	u32 magic;
	u32 entryCount;
};

struct AnimDef_Entry {
	u16 texNameOffset;
	u16 frameDelayOffset;
	u16 tileNum;
	u8 tilesetNum;
	u8 reverse;
};

FileHandle fh;

void DoTiles(void* self) {
	AnimDef_Header *header;
	
	header = (AnimDef_Header*)LoadFile(&fh, "/NewerRes/AnimTiles.bin");
	
	if (!header) {
		OSReport("anim load fail\n");
		return;
	}
	
	if (header->magic != 'NWRa') {
		OSReport("anim info incorrect\n");
		FreeFile(&fh);
		return;
	}
	
	AnimDef_Entry *entries = (AnimDef_Entry*)(header+1);
	
	for (int i = 0; i < header->entryCount; i++) {
		AnimDef_Entry *entry = &entries[i];
		char *name = (char*)fh.filePtr+entry->texNameOffset;
		char *frameDelays = (char*)fh.filePtr+entry->frameDelayOffset;
		
		char realName[0x40];
		snprintf(realName, 0x40, "BG_tex/%s", name);
		
		void *blah = BgTexMng__LoadAnimTile(self, entry->tilesetNum, entry->tileNum, realName, frameDelays, entry->reverse);
	}
}


void DestroyTiles(void *self) {
	FreeFile(&fh);
}


extern "C" void CopyAnimTile(u8 *target, int tileNum, u8 *source, int frameNum) {
	int tileRow = tileNum >> 5; // divided by 32
	int tileColumn = tileNum & 31; // modulus by 32

	u8 *baseRow = target + (tileRow * 2 * 32 * 1024);
	u8 *baseTile = baseRow + (tileColumn * 32 * 4 * 2);

	u8 *sourceRow = source + (frameNum * 2 * 32 * 32);

	for (int i = 0; i < 8; i++) {
		memcpy(baseTile, sourceRow, 32*4*2);
		baseTile += (2 * 4 * 1024);
		sourceRow += (2 * 32 * 4);
	}
}

//
// processed\../src/bowserKey.cpp
//

#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include <stage.h>
#include "boss.h"

int KeyCounter = 0;
extern "C" void *KeyLoopSet(dStageActor_c *);

void endKeyLevel(dStageActor_c *key) {
	OSReport("Key End Level");

	if (key->settings) {
		KeyCounter += 1;
		OSReport("Key Counter now at: %d", KeyCounter);
		ExitStage(WORLD_MAP, 0, BEAT_LEVEL, MARIO_WIPE);
	}
}

void startEndKeyLevel(dStageActor_c *key) {
	key->speed.y = 4.0;
	OSReport("Key Set Speed");

	if (key->settings) {
		StopBGMMusic();
		PlaySound(key, STRM_BGM_COURSE_CLEAR_ZORO);
		MakeMarioEnterDemoMode();
	}
}

void soundsEndKeyLevel(dStageActor_c *key) {
	OSReport("Key Set Loop");
	KeyLoopSet(key);

	if (key->settings) {
		UpdateGameMgr();
		if (GetSpecificPlayerActor(0) != 0) { PlaySound(key, SE_VOC_MA_CLEAR_MULTI); }
		if (GetSpecificPlayerActor(1) != 0) { PlaySound(key, SE_VOC_LU_CLEAR_MULTI); }
		if (GetSpecificPlayerActor(2) != 0) { PlaySound(key, SE_VOC_KO_CLEAR_MULTI); }
		if (GetSpecificPlayerActor(3) != 0) { PlaySound(key, SE_VOC_KO2_CLEAR_MULTI); }
	}
}



//
// processed\../src/levelspecial.cpp
//

#include <common.h>
#include <game.h>
#include <dCourse.h>

struct LevelSpecial {
	u32 id;			// 0x00
	u32 settings;	// 0x04
	u16 name;		// 0x08
	u8 _0A[6];		// 0x0A
	u8 _10[0x9C];	// 0x10
	float x;		// 0xAC
	float y;		// 0xB0
	float z;		// 0xB4
	u8 _B8[0x318];	// 0xB8
	// Any variables you add to the class go here; starting at offset 0x3D0
	u64 eventFlag;	// 0x3D0
	u8 type;		// 0x3D4
	u8 effect;		// 0x3D5
	u8 lastEvState;	// 0x3D6
	u8 func;		// 0x3D7
	u32 keepTime;
	u32 setTime;
};


extern u16 TimeStopFlag;
extern u32 AlwaysDrawFlag;
extern u32 AlwaysDrawBranch;

extern float MarioDescentRate;
extern float MarioJumpMax;
extern float MarioJumpArc;
extern float MiniMarioJumpArc;
// extern float MarioSize;

extern float GlobalSpriteSize;
extern float GlobalSpriteSpeed;
extern float GlobalRiderSize;
extern char SizerOn;
extern char ZOrderOn;
extern int GlobalStarsCollected;

extern VEC2 BGScaleFront;
extern VEC2 BGScaleBack;
extern char BGScaleEnabled;

extern u32 GameTimer;

extern char CameraLockEnabled;
extern VEC2 CameraLockPosition;
extern char isLockPlayerRotation;

#define time *(u32*)((GameTimer) + 0x4)


static const float GlobalSizeFloatModifications [] = {1, 0.25, 0.5, 0.75, 1.25, 1.5, 1.75, 2, 2.5, 3, 4, 5, 6, 7, 8, 10 };
static const float GlobalRiderFloatModifications [] = {1, 0.6, 0.7, 0.9, 1, 1, 1, 1.1, 1.25, 1.5, 2, 2.5, 3, 3.5, 4, 5};
static const float BGScaleChoices[] = {0.1f, 0.15f, 0.25f, 0.375f, 0.5f, 0.625f, 0.75f, 0.9f, 1.0f, 1.125f, 1.25f, 1.5f, 1.75f, 2.0f, 2.25f, 2.5f};

bool NoMichaelBuble = false;

void LevelSpecial_Update(LevelSpecial *self);
bool ResetAfterLevel();

#define ACTIVATE	1
#define DEACTIVATE	0

fBase_c *FindActorByID(u32 id);


extern "C" void dAcPy_vf294(void *Mario, dStateBase_c *state, u32 unk);
void MarioStateChanger(void *Mario, dStateBase_c *state, u32 unk) {
	//OSReport("State: %p, %s", state, state->getName());

	if ((strcmp(state->getName(), "dAcPy_c::StateID_Balloon") == 0) && (NoMichaelBuble)) { return; }

	dAcPy_vf294(Mario, state, unk);
}

bool ResetAfterLevel(bool didItWork) {
	// TimeStopFlag = 0;
	MarioDescentRate = -4;
	MarioJumpMax = 3.628;
	MarioJumpArc = 2.5;
	MiniMarioJumpArc = 2.5;
	// MarioSize = 1.0;
	GlobalSpriteSize = 1.0;
	GlobalSpriteSpeed = 1.0;
	GlobalRiderSize = 1.0;
	SizerOn = 0;
	AlwaysDrawFlag = 0x9421FFF0;
	AlwaysDrawBranch = 0x7C0802A6;
	ZOrderOn = 0;
	GlobalStarsCollected = 0;
	NoMichaelBuble = false;
	BGScaleEnabled = 0;
	CameraLockEnabled = 0;
	isLockPlayerRotation = false;
	return didItWork;
}

void FuckinBubbles() {
	dCourse_c *course = dCourseFull_c::instance->get(GetAreaNum());
	bool thing = false;

	int zone = GetZoneNum();
	for (int i = 0; i < course->zoneSpriteCount[zone]; i++) {
		dCourse_c::sprite_s *spr = &course->zoneFirstSprite[zone][i];
		if (spr->type == 246 && (spr->settings & 0xF) == 8)
			thing = true;
	}

	if (thing) {
		OSReport("DISABLING EXISTING BUBBLES.\n");
		for (int i = 0; i < 4; i++)
			Player_Flags[i] &= ~4;
	}
}

bool LevelSpecial_Create(LevelSpecial *self) {
	char eventNum	= (self->settings >> 24)	& 0xFF;
	self->eventFlag = (u64)1 << (eventNum - 1);
	
	self->keepTime  = 0;
	
	self->type		= (self->settings)			& 15;
	self->effect	= (self->settings >> 4)		& 15;
	self->setTime	= (self->settings >> 8)     & 0xFFFF;

	self->lastEvState = 0xFF;
	
	LevelSpecial_Update(self);
	
	return true;
}

bool LevelSpecial_Execute(LevelSpecial *self) {
	if (self->keepTime > 0) {
		time = self->keepTime; }

	LevelSpecial_Update(self);
	return true;
}


void LevelSpecial_Update(LevelSpecial *self) {
	
	u8 newEvState = 0;
	if (dFlagMgr_c::instance->flags & self->eventFlag)
		newEvState = 1;
	
	if (newEvState == self->lastEvState)
		return;
		
	
	u8 offState;
	if (newEvState == ACTIVATE)
	{
		offState = (newEvState == 1) ? 1 : 0;

		switch (self->type) {
			// case 1:											// Time Freeze
			// 	TimeStopFlag = self->effect * 0x100;
			// 	break;
				
			case 2:											// Stop Timer
				self->keepTime  = time;
				break;
		
	
			case 3:											// Mario Gravity
				if (self->effect == 0)
				{											//Low grav
					MarioDescentRate = -2;
					MarioJumpArc = 0.5;
					MiniMarioJumpArc = 0.5;
					MarioJumpMax = 4.5;
				}
				else
				{											//Anti-grav
					MarioDescentRate = 0.5;
					MarioJumpArc = 4.0;
					MiniMarioJumpArc = 4.0;
					MarioJumpMax = 0.0;
				}
				break;
	
			case 4:											// Set Time
				time = (self->setTime << 0xC) - 1; // Possibly - 0xFFF?
				break;


			case 5:											// Global Enemy Size
				SizerOn = 3;

				GlobalSpriteSize = GlobalSizeFloatModifications[self->effect];
				GlobalRiderSize = GlobalRiderFloatModifications[self->effect];
				GlobalSpriteSpeed = GlobalRiderFloatModifications[self->effect];

				AlwaysDrawFlag = 0x38600001;
				AlwaysDrawBranch = 0x4E800020;
				break;
	
			case 6:											// Individual Enemy Size
				AlwaysDrawFlag = 0x38600001;
				AlwaysDrawBranch = 0x4E800020;

				if (self->effect == 0)
				{	
					SizerOn = 1;							// Nyb 5
				}
				else
				{											
					SizerOn = 2;							// Nyb 7
				}
				break;
		
			case 7:											// Z Order Hack
				ZOrderOn = 1;
				break;

			case 8:
				NoMichaelBuble = true;
				break;

			case 9:
				BGScaleEnabled = true;
				BGScaleFront.x = BGScaleChoices[(self->settings >> 20) & 15];
				BGScaleFront.y = BGScaleChoices[(self->settings >> 16) & 15];
				BGScaleBack.x = BGScaleChoices[(self->settings >> 12) & 15];
				BGScaleBack.y = BGScaleChoices[(self->settings >> 8) & 15];
				break;

			default:
				break;
		}
	}
	
	else
	{
		offState = (newEvState == 1) ? 0 : 1;

		switch (self->type) {
			// case 1:											// Time Freeze
			// 	TimeStopFlag = 0;
			// 	break;
				
			case 2:											// Stop Timer
				self->keepTime  = 0;
				break;
		
	
			case 3:											// Mario Gravity
				MarioDescentRate = -4;
				MarioJumpArc = 2.5;
				MiniMarioJumpArc = 2.5;
				MarioJumpMax = 3.628;
				break;
	
			case 4:											// Mario Size
				break;
		
			case 5:											// Global Enemy Size
				SizerOn = 0;

				GlobalSpriteSize = 1.0;
				GlobalRiderSize = 1.0;
				GlobalSpriteSpeed = 1.0;

				AlwaysDrawFlag = 0x9421FFF0;
				AlwaysDrawBranch = 0x7C0802A6;
				break;

			case 6:											// Individual Enemy Size
				SizerOn = 0;

				AlwaysDrawFlag = 0x9421FFF0;
				AlwaysDrawBranch = 0x7C0802A6;
				break;
		
			case 7:											// Z Order Hack
				ZOrderOn = 0;
				break;
				
			case 8:
				NoMichaelBuble = false;
				break;

			case 9:
				BGScaleEnabled = false;
				break;
	
			default:
				break;
		}
	}




	
	
	self->lastEvState = newEvState;
}

//
// processed\../src/mrsun.cpp
//

#include <common.h>
#include <game.h>
#include <g3dhax.h>

#include "boss.h"

const char* MSarcNameList [] = {
	"mrsun",
	NULL	
};

class daMrSun_c : public dEn_c {
	int onCreate();
	int onDelete();
	int onExecute();
	int onDraw();

	mHeapAllocator_c allocator;
	m3d::mdl_c bodyModel;
	m3d::mdl_c glowModel;

	bool hasGlow;

	float Baseline;
	float SwoopSlope;
	float SpiralLoop;
	float yThreshold;
	float yAccel;
	Vec	swoopTarget;
	u32 timer;
	float xSpiralOffset;
	float ySpiralOffset;
	float swoopA;
	float swoopB;
	float swoopC;
	float swoopSpeed;
	float glowPos;
	short spinReduceZ;
	short spinReduceY;
	float spinStateOn;
	int dying;
	char sunDying;
	char killFlag;

	u64 eventFlag;


	void dieFall_Execute();
	static daMrSun_c *build();

	void updateModelMatrices();

	void playerCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat5_Mario(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther);

	USING_STATES(daMrSun_c);
	DECLARE_STATE(Follow);
	DECLARE_STATE(Swoop);
	DECLARE_STATE(Spiral);
	DECLARE_STATE(Spit);
	DECLARE_STATE(Spin);
	DECLARE_STATE(Wait);
};

daMrSun_c *daMrSun_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daMrSun_c));
	return new(buffer) daMrSun_c;
}


CREATE_STATE(daMrSun_c, Follow);
CREATE_STATE(daMrSun_c, Swoop);
CREATE_STATE(daMrSun_c, Spiral);
CREATE_STATE(daMrSun_c, Spit);
CREATE_STATE(daMrSun_c, Spin);
CREATE_STATE(daMrSun_c, Wait);

#define ACTIVATE	1
#define DEACTIVATE	0




void daMrSun_c::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther) {  DamagePlayer(this, apThis, apOther); }

bool daMrSun_c::collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther) {
	return true;
}
bool daMrSun_c::collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther) { 
	
	if (this->settings == 1) {  // It's a moon
		if (apOther->owner->name == 0x76) { // BROS_ICEBALL
			return true; 
			}
	}
	return false;
}
bool daMrSun_c::collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther) { 
	this->timer = 0; 
	PlaySound(this, SE_EMY_DOWN);
	doStateChange(&StateID_DieFall);
	return true;
}
bool daMrSun_c::collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther) { 
	this->timer = 0;
	PlaySound(this, SE_EMY_DOWN);
	doStateChange(&StateID_DieFall);
	return true;
}
bool daMrSun_c::collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther) { 
	this->timer = 0; 
	PlaySound(this, SE_EMY_DOWN);
	doStateChange(&StateID_DieFall);
	return true;
}
bool daMrSun_c::collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther) {
	DamagePlayer(this, apThis, apOther);
	return true;
}
bool daMrSun_c::collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther) {
	DamagePlayer(this, apThis, apOther);
	return true;
}
bool daMrSun_c::collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther) {
	DamagePlayer(this, apThis, apOther);
	return true;
}
bool daMrSun_c::collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther) {
	DamagePlayer(this, apThis, apOther);
	return true;
}
bool daMrSun_c::collisionCat5_Mario(ActivePhysics *apThis, ActivePhysics *apOther) {
	DamagePlayer(this, apThis, apOther);
	return true;
}


void daMrSun_c::dieFall_Execute() {
	
	if (this->killFlag == 1) { return; }

	this->timer = this->timer + 1;
	 
	this->dying = this->dying + 0.15;
	
	this->pos.x = this->pos.x + 0.15;
	this->pos.y = this->pos.y - ((-0.2 * (this->dying*this->dying)) + 5);
	
	this->dEn_c::dieFall_Execute();
		
	if (this->timer > 450) {
		
		if ((this->settings >> 28) > 0) { 		
			this->kill();
			this->pos.y = this->pos.y + 800.0; 
			this->killFlag = 1;
			return;
		}
		
		dStageActor_c *Player = GetSpecificPlayerActor(0);
		if (Player == 0) { Player = GetSpecificPlayerActor(1); }
		if (Player == 0) { Player = GetSpecificPlayerActor(2); }
		if (Player == 0) { Player = GetSpecificPlayerActor(3); }
		
	
		if (Player == 0) { 
			this->pos.x = 0;
			doStateChange(&StateID_Follow); }
		else {
			Player->pos;
			this->pos.x = Player->pos.x - 300;
		}
				
		this->pos.y = this->Baseline; 
		
		this->aPhysics.addToList();
		doStateChange(&StateID_Follow);
	}
}


int daMrSun_c::onCreate() {
	allocator.link(-1, GameHeaps[0], 0, 0x20);

	if ((this->settings & 0xF) == 0) { // It's a sun
		hasGlow = true;

		nw4r::g3d::ResFile rf(getResource("mrsun", "g3d/sun.brres"));
		bodyModel.setup(rf.GetResMdl("Sun"), &allocator, 0x224, 1, 0);
		SetupTextures_Map(&bodyModel, 0);

		glowModel.setup(rf.GetResMdl("SunGlow"), &allocator, 0x224, 1, 0);
		SetupTextures_Boss(&glowModel, 0);
	}
	
	else { // It's a moon
		hasGlow = false;

		nw4r::g3d::ResFile rf(getResource("mrsun", "g3d/moon.brres"));
		bodyModel.setup(rf.GetResMdl("Moon"), &allocator, 0x224, 1, 0);
		SetupTextures_Map(&bodyModel, 0);
	}
	
	allocator.unlink();

	this->scale = (Vec){0.5, 0.5, 0.5};


	ActivePhysics::Info HitMeBaby;
	HitMeBaby.xDistToCenter = 0.0;
	HitMeBaby.yDistToCenter = 0.0;
	HitMeBaby.category1 = 0x3;
	HitMeBaby.category2 = 0x0;
	HitMeBaby.bitfield1 = 0x6F;

	if ((this->settings & 0xF) == 0) { // It's a sun
		HitMeBaby.bitfield2 = 0xffbafffc; 
		HitMeBaby.xDistToEdge = 24.0;
		HitMeBaby.yDistToEdge = 24.0;
	}	
	else { // It's a moon
		HitMeBaby.bitfield2 = 0xffbafffe; 
		HitMeBaby.xDistToEdge = 12.0;
		HitMeBaby.yDistToEdge = 12.0;
	}

	HitMeBaby.unkShort1C = 0;
	HitMeBaby.callback = &dEn_c::collisionCallback;


	this->aPhysics.initWithStruct(this, &HitMeBaby);
	this->aPhysics.addToList();

	this->Baseline = this->pos.y;
	this->SwoopSlope = 0.0;
	this->SpiralLoop = 0;
	this->yThreshold = 15.0;
	this->yAccel = 0.2;
	this->timer = 0;
	this->xSpiralOffset = 0.0;
	this->ySpiralOffset = 0.0;
	this->dying = -5;
	this->sunDying = 0;
	this->killFlag = 0;
	
	if (this->settings == 1)
		this->pos.z = 6000.0f; // moon
	else
		this->pos.z = 5750.0f; // sun


	char eventNum	= (this->settings >> 16) & 0xFF;

	this->eventFlag = (u64)1 << (eventNum - 1);


	
	doStateChange(&StateID_Follow);

	// this->onExecute();
	return true;
}

int daMrSun_c::onDelete() {
	return true;
}

int daMrSun_c::onExecute() {
	acState.execute();
	updateModelMatrices();
		
	if (dFlagMgr_c::instance->flags & this->eventFlag) {
		if (this->killFlag == 0 && acState.getCurrentState()->isNotEqual(&StateID_DieFall)) {
			this->kill();
			this->pos.y = this->pos.y + 800.0; 
			this->killFlag = 1;
			doStateChange(&StateID_DieFall);
		}
	}
		
	return true;
}

int daMrSun_c::onDraw() {
	bodyModel.scheduleForDrawing();
	if (hasGlow)
		glowModel.scheduleForDrawing();

	return true;
}


void daMrSun_c::updateModelMatrices() {
	// This won't work with wrap because I'm lazy.
	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	bodyModel.setDrawMatrix(matrix);
	bodyModel.setScale(&scale);
	bodyModel.calcWorld(false);

	if (hasGlow) {
		mMtx glowMatrix;
		short rotY;
		
		glowPos += 0.01666666666666;
		if (glowPos > 1) { glowPos = 0; }
		
		rotY = (1000 * sin(glowPos * 3.14)) + 500;


		glowMatrix.translation(pos.x, pos.y, pos.z);
		glowMatrix.applyRotationX(&rot.x);
		glowMatrix.applyRotationY(&rotY);

		glowModel.setDrawMatrix(glowMatrix);
		glowModel.setScale(&scale);
		glowModel.calcWorld(false);
	}
}


// Follow State

void daMrSun_c::beginState_Follow() { 
	this->timer = 0;
	this->rot.x = 18000;
	this->rot.y = 0;
	this->rot.z = 0;
}
void daMrSun_c::executeState_Follow() { 

	if (this->timer > 200) { this->doStateChange(&StateID_Wait); }

	this->direction = dSprite_c__getXDirectionOfFurthestPlayerRelativeToVEC3(this, this->pos);
	
	float speedDelta;
	if ((this->settings & 0xF) == 0) { speedDelta = 0.1; } // It's a sun
	else { speedDelta = 0.15; } // It's a moon


	if (this->direction == 0) {
		this->speed.x = this->speed.x + speedDelta;
		
		if (this->speed.x < 0) { this->speed.x = this->speed.x + (speedDelta / 2); }
		if (this->speed.x < 6.0) { this->speed.x = this->speed.x + (speedDelta); }
	}
	else {
		this->speed.x = this->speed.x - speedDelta;

		if (this->speed.x > 0) { this->speed.x = this->speed.x - (speedDelta / 2); }
		if (this->speed.x > 6.0) { this->speed.x = this->speed.x - (speedDelta); }
	}
	
	this->HandleXSpeed();
	
	
	float yDiff;
	yDiff = (this->Baseline - this->pos.y) / 8;
	this->speed.y = yDiff;
		
	this->HandleYSpeed();

	this->UpdateObjectPosBasedOnSpeedValuesReal();

	this->timer = this->timer + 1;
}
void daMrSun_c::endState_Follow() { 
	this->speed.y = 0;
}


// Swoop State

void daMrSun_c::beginState_Swoop() { 
	
	// Not enough space to swoop, spit instead.
	if (this->swoopTarget.y < (this->pos.y - 50)) { doStateChange(&StateID_Spit); }
	if (((this->pos.x - 96) < this->swoopTarget.x) && (this->swoopTarget.x < (this->pos.x + 96))) { doStateChange(&StateID_Spit); }

	if ((this->settings & 0xF) == 0) { 
		this->swoopTarget.y = this->swoopTarget.y - 16;
	} // It's a sun
	
	else { 
		this->swoopTarget.y = this->swoopTarget.y - 4;
	} // It's a moon	
	
	
	float x1, x2, x3, y1, y2, y3;

	x1 = this->pos.x - this->swoopTarget.x;
	x2 = 0;
	x3 = -x1;

	y1 = this->pos.y - this->swoopTarget.y;
	y2 = 0;
	y3 = y1;
	
	float denominator = (x1 - x2) * (x1 - x3) * (x2 - x3);
	this->swoopA      = (x3 * (y2 - y1) + x2 * (y1 - y3) + x1 * (y3 - y2)) / denominator;
	this->swoopB      = (x3*x3 * (y1 - y2) + x2*x2 * (y3 - y1) + x1*x1 * (y2 - y3)) / denominator;
	this->swoopC      = (x2 * x3 * (x2 - x3) * y1 + x3 * x1 * (x3 - x1) * y2 + x1 * x2 * (x1 - x2) * y3) / denominator;

	this->swoopSpeed = x3 * 2 / 75;
	
	
	PlaySound(this, 284);
	
}
void daMrSun_c::executeState_Swoop() { 

	// Everything is calculated up top, just need to modify it.

	this->pos.x = this->pos.x + this->swoopSpeed;

	this->pos.y = ( this->swoopA*(this->pos.x - this->swoopTarget.x)*(this->pos.x - this->swoopTarget.x) + this->swoopB*(this->pos.x - this->swoopTarget.x) + this->swoopC ) + this->swoopTarget.y;

	if (this->pos.y > this->Baseline) { doStateChange(&StateID_Follow); }

}
void daMrSun_c::endState_Swoop() { 
	this->speed.y = 0;
}



// Spiral State

void daMrSun_c::beginState_Spiral() { 

	this->SpiralLoop = 0;
	this->xSpiralOffset = this->pos.x;
	this->ySpiralOffset = this->pos.y;

	PlaySound(this, 284);
}
void daMrSun_c::executeState_Spiral() { 

	float Loops;
	float Magnitude;
	float Period;

	Loops = 6.0;
	Magnitude = 11.0;

	// Use a period of 0.1 for the moon
	if ((this->settings & 0xF) == 0) { Period = 0.1; } // It's a sun
	else { Period = 0.125; } // It's a moon	

	this->pos.x = this->xSpiralOffset + Magnitude*((this->SpiralLoop * cos(this->SpiralLoop)));
	this->pos.y = this->ySpiralOffset + Magnitude*((this->SpiralLoop * sin(this->SpiralLoop)));

	this->SpiralLoop = this->SpiralLoop + Period;

	if (this->SpiralLoop > (3.14 * Loops)) { doStateChange(&StateID_Follow); }

}
void daMrSun_c::endState_Spiral() { }



// Spit State

void daMrSun_c::beginState_Spit() { 

	this->timer = 0;
	this->spinStateOn = 1;

}
void daMrSun_c::executeState_Spit() { 
	
	if (this->timer == 10) {

		PlaySound(this, 431);
	
		this->direction = dSprite_c__getXDirectionOfFurthestPlayerRelativeToVEC3(this, this->pos);
		
		float neg = -1.0;
		if (this->direction == 0) { neg = 1.0; }
		

		if ((this->settings & 0xF) == 0) { 
			dStageActor_c *spawner = CreateActor(106, 0, this->pos, 0, 0);
			spawner->speed.x = 6.0 * neg;
			spawner->speed.y = -2.5;
			spawner->pos.z = 5550.0;
			
			spawner = CreateActor(106, 0, this->pos, 0, 0);
			spawner->speed.x = 0.0 * neg;
			spawner->speed.y = -6.0;
			spawner->pos.z = 5550.0;
		
			spawner = CreateActor(106, 0, this->pos, 0, 0);
			spawner->speed.x = 3.5 * neg;
			spawner->speed.y = -6.0;
			spawner->pos.z = 5550.0;
		} // It's a sun
		
		
		else { 
			dStageActor_c *spawner = CreateActor(118, 0, this->pos, 0, 0);
			spawner->speed.x = 6.0 * neg;
			spawner->speed.y = -2.5;
			spawner->pos.z = 5550.0;
			*((u32 *) (((char *) spawner) + 0x3DC)) = this->id;
			
			spawner = CreateActor(118, 0, this->pos, 0, 0);
			spawner->speed.x = 0.0 * neg;
			spawner->speed.y = -6.0;
			spawner->pos.z = 5550.0;
			*((u32 *) (((char *) spawner) + 0x3DC)) = this->id;
		
			spawner = CreateActor(118, 0, this->pos, 0, 0);
			spawner->speed.x = 3.5 * neg;
			spawner->speed.y = -6.0;
			spawner->pos.z = 5550.0;
			*((u32 *) (((char *) spawner) + 0x3DC)) = this->id;
		} // It's a moon	

	}
	
	this->timer = this->timer + 1;

	if (this->timer > 30) { doStateChange(&StateID_Follow); }

}
void daMrSun_c::endState_Spit() { 
	this->spinStateOn = 0;
}



// Spin State

void daMrSun_c::beginState_Spin() { 
	this->spinReduceZ = 0;
	this->spinReduceY = 0;
}
void daMrSun_c::executeState_Spin() { 
	
	PlaySound(this, 282);

	this->direction = dSprite_c__getXDirectionOfFurthestPlayerRelativeToVEC3(this, this->pos);
	
	if (this->direction == 0) {
		this->speed.x = this->speed.x + 0.2;

		if (this->speed.x < 0) { this->speed.x = this->speed.x + (0.2 / 2); }
		if (this->speed.x < 80.0) { this->speed.x = this->speed.x + (0.2 * 2); }
	}
	else {
		this->speed.x = this->speed.x - 0.2;

		if (this->speed.x > 0) { this->speed.x = this->speed.x - (0.2 / 2); }
		if (this->speed.x > 80.0) { this->speed.x = this->speed.x - (0.2 * 2); }
	}
	
	this->HandleXSpeed();
	this->UpdateObjectPosBasedOnSpeedValuesReal();

	this->timer = this->timer + 1;

	short rotBonus;
	if (this->timer < 60) { rotBonus = this->timer; }
	else { rotBonus = 120 - this->timer; }

	// 1-59 + 60-1 = 3542
		
//	if (this->timer > 100) { 
//		if (this->spinReduceZ = 0) { 
//			this->spinReduceZ = this->rot.z / 20; }
//		if (this->spinReduceY = 0) { 
//			this->spinReduceY = this->rot.y / 20; }
//			
//		this->rot.z = this->rot.z - this->spinReduceZ;
//		this->rot.y = this->rot.y - this->spinReduceY; }
//		
//	else {
		this->rot.z = this->rot.z + (55.1 * rotBonus);
		this->rot.y = this->rot.y + (18.4 * rotBonus); //}


	float spitspeed;
	if ((this->settings & 0xF) == 0) { spitspeed = 3.0; } // It's a sun
	else { spitspeed = 4.0;  } // It's a moon	

	int randomBall;
	randomBall = GenerateRandomNumber(8);
	if (randomBall == 1) {
		int direction;
		direction = GenerateRandomNumber(8);
		
		float xlaunch;
		float ylaunch;
		
		if (direction == 0) { 
			xlaunch = spitspeed;
			ylaunch = 0.0; }
		else if (direction == 1) { // SE
			xlaunch = spitspeed;
			ylaunch = spitspeed; }
		else if (direction == 2) { // S
			xlaunch = 0.0;
			ylaunch = spitspeed; }
		else if (direction == 3) { // SW
			xlaunch = -spitspeed;
			ylaunch = spitspeed; }
		else if (direction == 4) {	// W
			xlaunch = -spitspeed;
			ylaunch = 0.0; }
		else if (direction == 5) {	// NW
			xlaunch = -spitspeed;
			ylaunch = -spitspeed; }
		else if (direction == 6) {	// N
			xlaunch = 0.0;
			ylaunch = -spitspeed; }
		else if (direction == 7) {	// NE
			xlaunch = spitspeed;
			ylaunch = -spitspeed; }
		
		PlaySound(this, 431);

		if ((this->settings & 0xF) == 0) { 
			dStageActor_c *spawner = CreateActor(106, 0, this->pos, 0, 0);
			spawner->speed.x = xlaunch;
			spawner->speed.y = ylaunch;
			spawner->pos.z = 5550.0;
		} // It's a sun

		else { 
			dStageActor_c *spawner = CreateActor(118, 0, this->pos, 0, 0);
			spawner->speed.x = xlaunch;
			spawner->speed.y = ylaunch;
			spawner->pos.z = 5550.0;
			
			*((u32 *) (((char *) spawner) + 0x3DC)) = this->id;			
		} // It's a moon	
	}

	if (this->timer > 120) { this->doStateChange(&StateID_Follow); }
	
}
void daMrSun_c::endState_Spin() { 

	this->rot.x = 18000;
	this->rot.y = 0;
	this->rot.z = 0;

	this->speed.x = 0;
}



// Wait State

void daMrSun_c::beginState_Wait() {


	this->timer = 0;
	this->speed.x = 0.0;

	dStageActor_c *Player = GetSpecificPlayerActor(0);
	if (Player == 0) { Player = GetSpecificPlayerActor(1); }
	if (Player == 0) { Player = GetSpecificPlayerActor(2); }
	if (Player == 0) { Player = GetSpecificPlayerActor(3); }
	if (Player == 0) { doStateChange(&StateID_Follow); }
	
	this->swoopTarget = Player->pos;
}
void daMrSun_c::executeState_Wait() { 
	int Choice;
	int TimerMax;
	
	if ((this->settings & 0xF) == 0) { TimerMax = 60; } // It's a sun
	else { TimerMax = 30; } // It's a moon	
	
	if (this->timer > TimerMax) {

		Choice = GenerateRandomNumber(9);


		if (Choice == 0) { doStateChange(&StateID_Spit); }
		else if (Choice == 1) { doStateChange(&StateID_Spit); }
		else if (Choice == 2) { doStateChange(&StateID_Spin); }
		else if (Choice == 3) { doStateChange(&StateID_Spiral); }
		else { doStateChange(&StateID_Swoop); }
		
	}

	this->timer = this->timer + 1;
}
void daMrSun_c::endState_Wait() {
	this->timer = 0;
}



//
// processed\../src/firelaser.cpp
//

#include <common.h>
#include <game.h>
#include <g3dhax.h>

class daFireLaser_c : public dEn_c {
	int onCreate();
	int onDelete();
	int onExecute();
	int onDraw();

	static daFireLaser_c *build();

	int timer;
	float spitspeed;
	char direction;
	u64 eventFlag;

	USING_STATES(daFireLaser_c);
	DECLARE_STATE(pewpewpew);
};

daFireLaser_c *daFireLaser_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daFireLaser_c));
	return new(buffer) daFireLaser_c;
}


CREATE_STATE(daFireLaser_c, pewpewpew);




int daFireLaser_c::onCreate() {

	this->timer = 0;
	this->direction = this->settings & 0xF;
	this->spitspeed = 8.0;
	
	char eventNum	= (this->settings >> 16) & 0xFF;
	this->eventFlag = (u64)1 << (eventNum - 1);

	
	doStateChange(&StateID_pewpewpew);
	this->onExecute();
	return true;
}

int daFireLaser_c::onDelete() {
	return true;
}

int daFireLaser_c::onExecute() {
	acState.execute();
	return true;
}

int daFireLaser_c::onDraw() {
	return true;
}



// Pew Pew State

void daFireLaser_c::beginState_pewpewpew() { 
	this->timer = 0;
}
void daFireLaser_c::executeState_pewpewpew() { 
	
	
	if (dFlagMgr_c::instance->flags & this->eventFlag) {
		
		this->timer = this->timer + 1;
	
		if (this->timer < 20) {
			float xlaunch;
			float ylaunch;
			
			if (this->direction == 0) { 
				xlaunch = this->spitspeed;
				ylaunch = 0.0; }
			else if (this->direction == 1) { // SE
				xlaunch = this->spitspeed;
				ylaunch = this->spitspeed; }
			else if (this->direction == 2) { // S
				xlaunch = 0.0;
				ylaunch = this->spitspeed; }
			else if (this->direction == 3) { // SW
				xlaunch = -this->spitspeed;
				ylaunch = this->spitspeed; }
			else if (this->direction == 4) {	// W
				xlaunch = -this->spitspeed;
				ylaunch = 0.0; }
			else if (this->direction == 5) {	// NW
				xlaunch = -this->spitspeed;
				ylaunch = -this->spitspeed; }
			else if (this->direction == 6) {	// N
				xlaunch = 0.0;
				ylaunch = -this->spitspeed; }
			else if (this->direction == 7) {	// NE
				xlaunch = this->spitspeed;
				ylaunch = -this->spitspeed; }
			
			
			dStageActor_c *spawner = CreateActor(106, 0, this->pos, 0, 0);
			spawner->speed.x = xlaunch;
			spawner->speed.y = ylaunch;
		}
		
		if (this->timer > 60) {
			this->timer = 0;
		}

	}
	
	else { this->timer = 0; }
	
}
void daFireLaser_c::endState_pewpewpew() { 

}














//
// processed\../src/poweruphax.cpp
//

#include <common.h>
#include <game.h>
#include <g3dhax.h>


void ThwompHammer(dEn_c *thwomp, ActivePhysics *apThis, ActivePhysics *apOther) {
	if (thwomp->name == 0x51) {
		thwomp->dEn_c::collisionCat13_Hammer(apThis, apOther);
	}
	return;
}

void BooHammer(dEn_c *boo, ActivePhysics *apThis, ActivePhysics *apOther) {
	if (boo->name == 0xB0) {
		boo->dEn_c::collisionCat13_Hammer(apThis, apOther);
	}
	return;
}

void UrchinHammer(dEn_c *urchin, ActivePhysics *apThis, ActivePhysics *apOther) {
	return;
}


#include "poweruphax.h"

void SetCullModeForMaterial(m3d::mdl_c* model, int materialID, u32 cullMode);

dHammerSuitRenderer_c* dHammerSuitRenderer_c::build() {
	return new dHammerSuitRenderer_c;
}

dHammerSuitRenderer_c::dHammerSuitRenderer_c() {}
dHammerSuitRenderer_c::~dHammerSuitRenderer_c() {}

void dHammerSuitRenderer_c::setup(dPlayerModelHandler_c* handler) {
	setup(handler, 0);
}

void dHammerSuitRenderer_c::setup(dPlayerModelHandler_c* handler, int sceneID) {
	victim = (dPlayerModel_c*)handler->mdlClass;

	allocator.link(-1, GameHeaps[0], 0, 0x20);

	// Carga modelo Hammer Suit
	nw4r::g3d::ResFile hammerRf(getResource("hammerM", "g3d/suit.brres"));
	if (victim->player_id_2 <= 1) {
		helmet.setup(hammerRf.GetResMdl((victim->player_id_2 == 0) ? "marioHelmet" : "luigiHelmet"), &allocator, 0, 1, 0);
		SetupTextures_MapObj(&helmet, sceneID);
	}
	const char* shellNames[] = { "shell", "shell", "shell", "shell", "shell" };
	shell.setup(hammerRf.GetResMdl(shellNames[victim->player_id_2]), &allocator, 0, 1, 0);
	SetupTextures_MapObj(&shell, sceneID);

	// Carga modelo Bomb Suit
	nw4r::g3d::ResFile bombRf(getResource("bombM", "g3d/bomb.brres"));
	bombModel.setup(bombRf.GetResMdl("bombModel"), &allocator, 0, 1, 0);
	SetupTextures_MapObj(&bombModel, sceneID);

	allocator.unlink();

	victimModel = &victim->models[0].body;
	nw4r::g3d::ResMdl* playerResMdl =
		(nw4r::g3d::ResMdl*)(((u32)victimModel->scnObj) + 0xE8);

	if (victim->player_id_2 <= 1) {
		nw4r::g3d::ResNode face_1 = playerResMdl->GetResNode("face_1");
		headNodeID = face_1.GetID();
	}

	nw4r::g3d::ResNode skl_root = playerResMdl->GetResNode("skl_root");
	rootNodeID = skl_root.GetID();
}

void dHammerSuitRenderer_c::draw() {
	if (victim->powerup_id == 7) {  // Hammer suit
		if (victim->player_id_2 <= 1) {
			SetCullModeForMaterial(&victim->getCurrentModel()->head, 3, GX_CULL_ALL);

			Mtx headMtx;
			victimModel->getMatrixForNode(headNodeID, &headMtx);

			helmet.setDrawMatrix(&headMtx);
			helmet.setScale(1.0f, 1.0f, 1.0f);
			helmet.calcWorld(false);

			helmet.scheduleForDrawing();
		}

		Mtx rootMtx;
		victimModel->getMatrixForNode(rootNodeID, &rootMtx);

		shell.setDrawMatrix(&rootMtx);
		shell.setScale(1.0f, 1.0f, 1.0f);
		shell.calcWorld(false);

		shell.scheduleForDrawing();
	}
	else if (victim->powerup_id == 8) {  // Bomb suit
		Mtx rootMtx;
		victimModel->getMatrixForNode(rootNodeID, &rootMtx);

		bombModel.setDrawMatrix(&rootMtx);
		bombModel.setScale(1.0f, 1.0f, 1.0f);
		bombModel.calcWorld(false);

		bombModel.scheduleForDrawing();
	}
}

// NEW VERSION
void CrapUpPositions(Vec *out, const Vec *in);

void dStockItem_c::setScalesOfSomeThings() {
	nw4r::lyt::Pane *ppos = N_forUse_PPos[playerCount];

	int howManyPlayers = 0;
	for (int i = 0; i < 4; i++) {
		if (isPlayerActive[i]) {
			int picID = getIconPictureIDforPlayer(howManyPlayers);
			int charID = Player_ID[i];

			if (picID != 24) {
				nw4r::lyt::Picture *pic = P_icon[picID];

				Vec in, out;

				in.x = pic->effectiveMtx[0][3];
				in.y = pic->effectiveMtx[1][3];
				in.z = pic->effectiveMtx[2][3];

				CrapUpPositions(&out, &in);

				u8 *wmp = (u8*)player2d[charID];
				*((float*)(wmp+0xAC)) = out.x;
				*((float*)(wmp+0xB0)) = out.y;
				*((float*)(wmp+0xB4)) = out.z;
				*((float*)(wmp+0x220)) = 0.89999998f;
				*((float*)(wmp+0x224)) = 0.89999998f;
				*((float*)(wmp+0x228)) = 0.89999998f;
				*((float*)(wmp+0x25C)) = 26.0f;
			}
			howManyPlayers++;
		}
	}


	for (int i = 0; i < 8; i++) {
		u8 *item = (u8*)newItemPtr[i];

		nw4r::lyt::Pane *icon = newIconPanes[i];

		Vec in, out;
		in.x = icon->effectiveMtx[0][3];
		in.y = icon->effectiveMtx[1][3];
		in.z = icon->effectiveMtx[2][3];

		CrapUpPositions(&out, &in);

		*((float*)(item+0xAC)) = out.x;
		*((float*)(item+0xB0)) = out.y;
		*((float*)(item+0xB4)) = out.z;
		*((float*)(item+0x1F4)) = P_buttonBase[i]->scale.x;
		*((float*)(item+0x1F8)) = P_buttonBase[i]->scale.y;
		*((float*)(item+0x1FC)) = 1.0f;
	}


	nw4r::lyt::Pane *shdRoot = shadow->rootPane;
	shdRoot->trans.x = N_stockItem->effectiveMtx[0][3];
	shdRoot->trans.y = N_stockItem->effectiveMtx[1][3];
	shdRoot->trans.z = N_stockItem->effectiveMtx[2][3];
	shdRoot->scale.x = N_stockItem_01->effectiveMtx[0][0];
	shdRoot->scale.y = N_stockItem_01->effectiveMtx[1][1];

	for (int i = 0; i < 7; i++)
		shadow->buttonBases[i]->scale = newButtonBase[i]->scale;
	shadow->hammerButtonBase->scale = newButtonBase[7]->scale;

}



//
// processed\../src/randtiles.cpp
//

#include <game.h>

class RandomTileData {
public:
	enum Type {
		CHECK_NONE = 0,
		CHECK_HORZ = 1,
		CHECK_VERT = 2,
		CHECK_BOTH = 3
	};

	enum Special {
		SP_NONE = 0,
		SP_VDOUBLE_TOP = 1,
		SP_VDOUBLE_BOTTOM = 2
	};

	class NameList {
	public:
		u32 count;
		u32 offsets[1]; // variable size

		const char *getName(int index) {
			return ((char*)this) + offsets[index];
		}

		bool contains(const char *name) {
			for (int i = 0; i < count; i++) {
				if (strcmp(name, getName(i)) == 0)
					return true;
			}

			return false;
		}
	};

	class Entry {
	public:
		u8 lowerBound, upperBound;
		u8 count, type;
		u32 tileNumOffset;

		u8 *getTileNums() {
			return ((u8*)this) + tileNumOffset;
		}
	};

	class Section {
	public:
		u32 nameListOffset;
		u32 entryCount;
		Entry entries[1]; // variable size

		NameList *getNameList() {
			return (NameList*)(((u32)this) + nameListOffset);
		}
	};

	u32 magic;
	u32 sectionCount;
	u32 offsets[1]; // variable size

	Section *getSection(int id) {
		return (Section*)(((char*)this) + offsets[id]);
	}

	Section *getSection(const char *name);

	static RandomTileData *instance;
};

class RTilemapClass : public TilemapClass {
public:
	// NEWER ADDITIONS
	RandomTileData::Section *sections[4];
};

RandomTileData::Section *RandomTileData::getSection(const char *name) {
	for (int i = 0; i < sectionCount; i++) {
		RandomTileData::Section *sect = getSection(i);

		if (sect->getNameList()->contains(name))
			return sect;
	}

	return 0;
}


// Real tile handling code

RandomTileData *RandomTileData::instance = 0;

dDvdLoader_c RandTileLoader;

// This is a bit hacky but I'm lazy
bool LoadLevelInfo();

extern "C" bool RandTileLoadHook() {
	// OSReport("Trying to load...");
	void *buf = RandTileLoader.load("/NewerRes/RandTiles.bin");
	bool LIresult = LoadLevelInfo();
	if (buf == 0) {
		// OSReport("Failed.\n");
		return false;
	} else {
		// OSReport("Successfully loaded RandTiles.bin [%p].\n", buf);
		RandomTileData::instance = (RandomTileData*)buf;
		return LIresult;
	}
}


extern "C" void IdentifyTilesets(RTilemapClass *self) {
	self->_C0C = 0xFFFFFFFF;

	for (int i = 0; i < 4; i++) {
		const char *tilesetName = BGDatClass::instance->getTilesetName(self->areaID, i);

		self->sections[i] = RandomTileData::instance->getSection(tilesetName);
		// OSReport("[%d] Chose %p for %s\n", i, self->sections[i], tilesetName);
	}
}

extern "C" void TryAndRandomise(RTilemapClass *self, BGRender *bgr) {
	int fullTile = bgr->tileToPlace & 0x3FF;
	int tile = fullTile & 0xFF;
	int tileset = fullTile >> 8;

	RandomTileData::Section *rtSect = self->sections[tileset];
	if (rtSect == 0)
		return;

	for (int i = 0; i < rtSect->entryCount; i++) {
		RandomTileData::Entry *entry = &rtSect->entries[i];

		if (tile >= entry->lowerBound && tile <= entry->upperBound) {
			// Found it!!
			// Try to make one until we meet the conditions
			u8 type = entry->type & 3;
			u8 special = entry->type >> 2;

			u8 *tileNums = entry->getTileNums();
			u16 chosen = 0xFF;

			// If it's the top special, then ignore this tile, we'll place that one
			// once we choose the bottom one
			if (special == RandomTileData::SP_VDOUBLE_TOP)
				break;

			u16 *top = 0, *left = 0, *right = 0, *bottom = 0;
			if (type == RandomTileData::CHECK_HORZ || type == RandomTileData::CHECK_BOTH) {
				left = self->getPointerToTile((bgr->curX - 1) * 16, bgr->curY * 16);
				right = self->getPointerToTile((bgr->curX + 1) * 16, bgr->curY * 16);
			}

			if (type == RandomTileData::CHECK_VERT || type == RandomTileData::CHECK_BOTH) {
				top = self->getPointerToTile(bgr->curX * 16, (bgr->curY - 1) * 16);
				bottom = self->getPointerToTile(bgr->curX * 16, (bgr->curY + 1) * 16);
			}

			int attempts = 0;
			while (true) {
				// is there even a point to using that special random function?
				chosen = (tileset << 8) | tileNums[MakeRandomNumberForTiles(entry->count)];

				// avoid infinite loops
				attempts++;
				if (attempts > 5)
					break;

				if (top != 0 && *top == chosen)
					continue;
				if (bottom != 0 && *bottom == chosen)
					continue;
				if (left != 0 && *left == chosen)
					continue;
				if (right != 0 && *right == chosen)
					continue;
				break;
			}

			bgr->tileToPlace = chosen;

			if (special == RandomTileData::SP_VDOUBLE_BOTTOM) {
				if (top == 0)
					top = self->getPointerToTile(bgr->curX * 16, (bgr->curY - 1) * 16);

				*top = (chosen - 0x10);
			}

			return;
		}
	}
}



//
// processed\../src/objkinoko.cpp
//

#include <game.h>
#include <g3dhax.h>


class SomethingAboutShrooms {
	public:
		m3d::mdl_c models[3];
		float scale, _C4, offsetToEdge;
		m3d::anmTexPat_c animations[3];

		struct info_s {
			const char *leftName;
			const char *middleName;
			const char *rightName;
			float size; // 8 for small, 16 for big
			const char *lrName;
			const char *middleNameAgain;
		};

		void setup(mAllocator_c *allocator,
				nw4r::g3d::ResFile *resFile, info_s *info,
				float length, float colour, float scale);

		// plus more methods I don't know

		void drawWithMatrix(float yOffset, mMtx *matrix);
};


class dRotatorThing_c {
	public:
		s16 _p5, output, _p1, _p2, _p3;
		s16 _p6, _p7, _p0;
		u32 someBool;

		void setup(s16 a, s16 b, s16 c, s16 d, s16 initialRotation, s16 f, s16 g, s16 h);
		s16 execute();
};


class daObjKinoko_c : public dStageActor_c {
	public:
		mHeapAllocator_c allocator;
		nw4r::g3d::ResFile resFile;
		SomethingAboutShrooms renderer;
		StandOnTopCollider colliders[3];
		dRotatorThing_c xRotator;
		dRotatorThing_c zRotator;
		u8 thickness, touchCompare;

		void loadModels(int thickness, float length, float colour, float scale);
		void addAllColliders();
		void updateAllColliders();
		void removeAllColliders();
		u8 checkIfTouchingObject();

		int onCreate();
		int onExecute();
		int onDraw();
		int onDelete();

		int creationHook();
		int drawHook();

		int original_onCreate();

		~daObjKinoko_c();
};


// This will replace Nintendo's onCreate
int daObjKinoko_c::creationHook() {
	original_onCreate();

	// if rotation is off, do nothing else
	if (!((settings >> 28) & 1))
		return 1;

	// OK, figure out the rotation
	u8 sourceRotation = (settings >> 24) & 0xF;

	// 0 is up. -0x4000 is right, 0x4000 is left ...
	s16 rotation;

	// We'll flip it later.
	// Thus: 0..7 rotates left (in increments of 0x800),
	// 8..15 rotates right (in increments of 0x800 too).
	// To specify facing up, well.. just use 0.

	if (sourceRotation < 8)
		rotation = (sourceRotation * 0x800) - 0x4000;
	else
		rotation = (sourceRotation * 0x800) - 0x3800;

	rotation = -rotation;

	rot.z = rotation;

	/* Original code: */
	int lengthInTiles = settings & 0xF;

	float sizeMult = (thickness == 1) ? 1.0f : 0.5f;
	float length = sizeMult * ((32.0f + (lengthInTiles * 16.0f)) - 16.0f);

	float topPos = (sizeMult * 16.0f);

	float cosThing = nw4r::math::CosFIdx((lengthInTiles * 16.0f) / 256.0f);
	float anotherThing = (4.0f + topPos) / cosThing;

	// Middle Collider
	colliders[0].init(this,
			/*xOffset=*/0.0f, /*yOffset=*/0.0f,
			/*topYOffset=*/topPos,
			/*rightSize=*/length, /*leftSize=*/-length,
			/*rotation=*/rotation, /*_45=*/1
			);

	colliders[0]._47 = 0;
	colliders[0].flags = 0x80180 | 0xC00;

	// Now get the info to move the colliders by ....
	float rotFIdx = ((float)rotation) / 256.0f;
	float sinRot, cosRot;
	nw4r::math::SinCosFIdx(&sinRot, &cosRot, rotFIdx);
	//OSReport("Rotation is %d, so rotFIdx is %f\n", rotation, rotFIdx);
	//OSReport("Sin: %f, Cos: %f\n", sinRot, cosRot);
	
	float leftXOffs = (cosRot * -length) - (sinRot * topPos);
	float leftYOffs = (sinRot * -length) + (cosRot * topPos);
	float rightXOffs = (cosRot * length) - (sinRot * topPos);
	float rightYOffs = (sinRot * length) + (cosRot * topPos);
	//OSReport("leftXOffs: %f, leftYOffs: %f\n", leftXOffs, leftYOffs);
	//OSReport("rightXOffs: %f, rightYOffs: %f\n", rightXOffs, rightYOffs);

	// Right Collider
	colliders[1].init(this,
			/*xOffset=*/rightXOffs, /*yOffset=*/rightYOffs,
			/*topYOffset=*/0.0f,
			/*rightSize=*/anotherThing, /*leftSize=*/0.0f,
			/*rotation=*/rotation - 0x2000, /*_45=*/1
			);

	colliders[1]._47 = 0;
	colliders[1].flags = 0x80100 | 0x800;

	// Left Collider
	colliders[2].init(this,
			/*xOffset=*/leftXOffs, /*yOffset=*/leftYOffs,
			/*topYOffset=*/0.0f,
			/*rightSize=*/0.0f, /*leftSize=*/-anotherThing,
			/*rotation=*/rotation + 0x2000, /*_45=*/1
			);

	colliders[2]._47 = 0;
	colliders[2].flags = 0x80080 | 0x400;

	return 1;
}


// This will replace Nintendo's onDraw
int daObjKinoko_c::drawHook() {
	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationZ(&rot.z);
	matrix.applyRotationX(&xRotator.output);
	matrix.applyRotationZ(&zRotator.output);
	renderer.drawWithMatrix(0.0f, &matrix);

	return 1;
}



//
// processed\../src/tilegod.cpp
//

#include <game.h>
#include <sfx.h>

//#define REIMPLEMENT

extern "C" bool SpawnEffect(const char*, int, Vec*, S16Vec*, Vec*);

class daChengeBlock_c : public dStageActor_c {
	static daChengeBlock_c *build();

	u32 _394;
	u64 initialFlags;

	enum Action { Destroy, Create };
	enum Pattern { Fill, CheckerA, CheckerB };

	int width, height;
	Action action;
	int blockType;
	int isPermanent;
	Pattern pattern;
	u16 hasTriggered;

	u32 _3BC;


	int onCreate();
	int onExecute();

	void doStuff(Action action, bool wasCalledOnCreation);
	void tryToTrigger();
};

#ifdef REIMPLEMENT
daChengeBlock_c *daChengeBlock_c::build() {
	return new(AllocFromGameHeap1(sizeof(daChengeBlock_c))) daChengeBlock_c;
}



int daChengeBlock_c::onCreate() {
	hasTriggered = 0;

	height = settings & 0xF;
	width = (settings & 0xF0) >> 4;
	blockType = (settings & 0xF000) >> 12;
	pattern = (Pattern)((settings & 0x30000) >> 16);
	isPermanent = (settings & 0xF0000000) >> 28;

	if (width == 0)
		width++;
	if (height == 0)
		height++;

	action = (Action)((settings & 0xF00) >> 8);

	initialFlags = dFlagMgr_c::instance->flags & dStageActor_c::creatingFlagMask;

	if (initialFlags) {
		if (action == Destroy) {
			doStuff(Destroy, true);
		} else {
			doStuff(Create, true);
			hasTriggered = true;
		}

		if (isPermanent)
			return 2;
	}

	return 1;
}


int daChengeBlock_c::onExecute() {
	tryToTrigger();

	if (!hasTriggered)
		checkZoneBoundaries(0);

	return 1;
}


#endif // REIMPLEMENT

// Red, Brick, Blank/Unused, Stone, Wood, Blank
static const u16 Tiles[] = {124, 2, 12, 123, 15, 0};

void daChengeBlock_c::doStuff(Action action, bool wasCalledOnCreation) {
	u16 perTilePatternFlag = 1, perRowPatternFlag = 1;

	u16 worldX = ((u16)pos.x) & 0xFFF0;
	u16 baseWorldX = worldX;
	u16 worldY = ((u16)(-pos.y)) & 0xFFF0;

	if (pattern == CheckerB) {
		perTilePatternFlag = 0;
		perRowPatternFlag = 0;
	}

	u16 tile;
	if (action != Destroy) {
		if (blockType & 8) {
			// Specify a tile number
			tile = 0x8000 | ((blockType & 3) << 8) | ((settings & 0xFF00000) >> 20);
		} else {
			// fall through
			tile = Tiles[blockType];
		}
	} else {
		tile = 0;
	}

	for (u16 y = 0; y < height; y++) {
		for (u16 x = 0; x < width; x++) {
			if (perTilePatternFlag) {
				u16 *pExistingTile = dBgGm_c::instance->getPointerToTile(worldX, worldY, currentLayerID);
				u16 existingTile = pExistingTile ? *pExistingTile : 0;

				dBgGm_c::instance->placeTile(worldX, worldY, currentLayerID, tile);

				if (!wasCalledOnCreation) {
					Vec effectPos;

					if (action == Destroy) {
						if (blockType != 2) {
							effectPos.x = ((float)(worldX)) + 8.0f;
							effectPos.y = ((float)(-worldY)) - 8.0f;
							effectPos.z = pos.z;

							u16 shardType;
							switch (existingTile) {
								case 0x30: shardType = 0; break;
								case 0x31: shardType = 3; break;
								case 0x32: shardType = 4; break;
								case 0x33: shardType = 2; break;
								case 0x34: shardType = 1; break;
								case 0x37: shardType = 5; break;
								default: shardType = 0xFFFF;
							}

							if (!(settings & 0x40000)) {
								if (shardType == 0xFFFF) {
									SpawnEffect("Wm_en_burst_ss", 0, &effectPos, 0, 0);
								} else {
									u32 sets = (shardType << 8) | 3;
									effectPos.y -= 8;
									dEffectBreakMgr_c::instance->spawnTile(&effectPos, sets, 0);
								}
							}

							if (!(settings & 0x80000)) {
								Vec2 soundPos;
								ConvertStagePositionToScreenPosition(&soundPos, &effectPos);
								SoundPlayingClass::instance2->PlaySoundAtPosition(SE_OBJ_BLOCK_BREAK, &soundPos, 0);
							}
						}
					} else {
						effectPos.x = ((float)(worldX)) + 8.0f;
						effectPos.y = ((float)(-worldY)) - 8.0f;
						effectPos.z = pos.z;

						if (!(settings & 0x40000)) {
							if (blockType != 2) {
								SpawnEffect("Wm_en_burst_ss", 0, &effectPos, 0, 0);
							}
						}
					}
				}
			}

			if (pattern != Fill) {
				perTilePatternFlag ^= 1;
			}

			worldX += 16;
		}

		if (pattern != Fill) {
			perRowPatternFlag ^= 1;
			perTilePatternFlag = perRowPatternFlag;
		}

		worldX = baseWorldX;
		worldY += 16;
	}
}


#ifdef REIMPLEMENT
void daChengeBlock_c::tryToTrigger() {
	u64 result = spriteFlagMask & dFlagMgr_c::instance->flags;

	if (action == Destroy) {
		if (result & initialFlags) {
			if (result) {
				doStuff(Destroy, false);
				hasTriggered = true;
			} else {
				doStuff(Create, false);
				hasTriggered = false;
			}

			initialFlags = result;

			if (isPermanent)
				fBase_c::Delete();
		}
	} else {
		if (result & initialFlags) {
			if (result) {
				doStuff(Create, false);
				hasTriggered = true;
			} else {
				doStuff(Destroy, false);
				hasTriggered = false;
			}

			initialFlags = result;

			if (isPermanent)
				fBase_c::Delete();
		}
	}
}
#endif


//
// processed\../src/linegod.cpp
//

#include <common.h>
#include <game.h>

// TODO: make "No Deactivation"

struct BgActor {
	u16 def_id;		// 0x00
	u16 x;			// 0x02
	u16 y;			// 0x04
	u8 layer;		// 0x06
	u8 EXTRA_off;	// 0x07
	u32 actor_id;	// 0x08
};

struct BgActorDef {
	u32 tilenum;
	u16 actor;
	u8 _06[2];
	float x;
	float y;
	float z;
	float width;
	float height;
	u32 extra_var;
};

struct dBgActorManager_c {
	u32 vtable;		// 0x00
	u8 _04[0x34];	// 0x04
	BgActor *array;	// 0x38
	u32 count;		// 0x3C
	u32 type;		// 0x40
};

extern dBgActorManager_c *dBgActorManager;

extern BgActorDef *BgActorDefs;

struct BG_GM_hax {
	u8 _00[0x8FE64];
	float _0x8FE64;
	float _0x8FE68;
	float _0x8FE6C;
	float _0x8FE70;
};

extern BG_GM_hax *BG_GM_ptr;

// Regular class is 0x3D0.
// Let's add stuff to the end just to be safe.
// Size is now 0x400
// 80898798 38600400

#define LINEGOD_FUNC_ACTIVATE	0
#define LINEGOD_FUNC_DEACTIVATE	1

struct LineGod {
	u32 id;			// 0x00
	u32 settings;	// 0x04
	u16 name;		// 0x08
	u8 _0A[6];		// 0x0A
	u8 _10[0x9C];	// 0x10
	float x;		// 0xAC
	float y;		// 0xB0
	float z;		// 0xB4
	u8 _B8[0x318];	// 0xB8
	u64 eventFlag;	// 0x3D0
	u8 func;		// 0x3D4
	u8 width;		// 0x3D5
	u8 height;		// 0x3D6
	u8 lastEvState;	// 0x3D7
	BgActor *ac[8];	// 0x3D8
};


fBase_c *FindActorByID(u32 id);

u16 *GetPointerToTile(BG_GM_hax *self, u16 x, u16 y, u16 layer, short *blockID_p, bool unused);



void LineGod_BuildList(LineGod *self);
bool LineGod_AppendToList(LineGod *self, BgActor *ac);
void LineGod_Update(LineGod *self);


bool LineGod_Create(LineGod *self) {
	char eventNum	= (self->settings >> 24)	& 0xFF;
	self->eventFlag = (u64)1 << (eventNum - 1);
	
	
	
	self->func		= (self->settings)			& 1;
	self->width		= (self->settings >> 4)		& 15;
	self->height	= (self->settings >> 8)		& 15;
	
	self->lastEvState = 0xFF;
	
	LineGod_BuildList(self);
	LineGod_Update(self);
	
	return true;
}

bool LineGod_Execute(LineGod *self) {
	LineGod_Update(self);
	return true;
}

void LineGod_BuildList(LineGod *self) {
	for (int clearIdx = 0; clearIdx < 8; clearIdx++) {
		self->ac[clearIdx] = 0;
	}
	
	

	float gLeft = self->x - (BG_GM_ptr->_0x8FE64 - fmod(BG_GM_ptr->_0x8FE64, 16));
	float gTop = self->y - (BG_GM_ptr->_0x8FE6C - fmod(BG_GM_ptr->_0x8FE6C, 16));

	// 1 unit padding to avoid catching stuff that is not in our rectangle
	Vec grect1 = (Vec){
		gLeft + 1, gTop - (self->height * 16) + 1, 0
	};

	Vec grect2 = (Vec){
		gLeft + (self->width * 16) - 1, gTop - 1, 0
	};

	
	for (int i = 0; i < dBgActorManager->count; i++) {
		BgActor *ac = &dBgActorManager->array[i];

		// the Def width/heights are padded with 8 units on each side
		// except for one of the steep slopes, which differs for no reason

		BgActorDef *def = &BgActorDefs[ac->def_id];
		float aXCentre = (ac->x * 16) + def->x;
		float aYCentre = (-ac->y * 16) + def->y;

		float xDistToCentre = (def->width - 16) / 2;
		float yDistToCentre = (def->height - 16) / 2;

		Vec arect1 = (Vec){
			aXCentre - xDistToCentre, aYCentre - yDistToCentre, 0
		};
		
		Vec arect2 = (Vec){
			aXCentre + xDistToCentre, aYCentre + yDistToCentre, 0
		};

		if (RectanglesOverlap(&arect1, &arect2, &grect1, &grect2))
			LineGod_AppendToList(self, ac);
	}
}

bool LineGod_AppendToList(LineGod *self, BgActor *ac) {
	
	for (int search = 0; search < 8; search++) {
		if (self->ac[search] == 0) {
			self->ac[search] = ac;
			return true;
		}
	}
	
	return false;
}

void LineGod_Update(LineGod *self) {
	
	u8 newEvState = 0;
	if (dFlagMgr_c::instance->flags & self->eventFlag)
		newEvState = 1;
	
	if (newEvState == self->lastEvState)
		return;
	
	u16 x_bias = (BG_GM_ptr->_0x8FE64 / 16);
	u16 y_bias = -(BG_GM_ptr->_0x8FE6C / 16);
	
	
	u8 offState;
	if (self->func == LINEGOD_FUNC_ACTIVATE)
		offState = (newEvState == 1) ? 1 : 0;
	else
		offState = (newEvState == 1) ? 0 : 1;
	
	
	for (int i = 0; i < 8; i++) {
		if (self->ac[i] != 0) {
			BgActor *ac = self->ac[i];
			
			
			ac->EXTRA_off = offState;
			if (offState == 1 && ac->actor_id != 0) {
				fBase_c *assoc_ac = FindActorByID(ac->actor_id);
				if (assoc_ac != 0)
					assoc_ac->Delete();
				ac->actor_id = 0;
			}
			
			u16 *tile = GetPointerToTile(BG_GM_ptr, (ac->x + x_bias) * 16, (ac->y + y_bias) * 16, 0, 0, 0);
			if (offState == 1)
				*tile = 0;
			else
				*tile = BgActorDefs[ac->def_id].tilenum;
			
		}
	}
	
	
	
	self->lastEvState = newEvState;
}

//
// processed\../src/tilesetfixer.cpp
//

#include <common.h>
#include <game.h>

const char *GetTilesetName(void *cls, int areaNum, int slotNum);

void DoFixes(int areaNumber, int slotNumber);
void SwapObjData(u8 *data, int slotNumber);

extern "C" void *OriginalTilesetLoadingThing(void *, void *, int, int);

// Main hook
void *TilesetFixerHack(void *cls, void *heap, int areaNum, int layerNum) {
	if (layerNum == 0) {
		for (int i = 1; i < 4; i++) {
			DoFixes(areaNum, i);
		}
	}

	return OriginalTilesetLoadingThing(cls, heap, areaNum, layerNum);
}



// File format definitions
struct ObjLookupEntry {
	u16 offset;
	u8 width;
	u8 height;
};


void DoFixes(int areaNumber, int slotNumber) {
	// This is where it all starts
	const char *tsName = GetTilesetName(BGDatClass, areaNumber, slotNumber);

	if (tsName == 0 || tsName[0] == 0) {
		return;
	}


	char untHDname[64], untname[64];
	snprintf(untHDname, 64, "BG_unt/%s_hd.bin", tsName);
	snprintf(untname, 64, "BG_unt/%s.bin", tsName);

	u32 unt_hd_length;
	void *bg_unt_hd_data = DVD_GetFile(GetDVDClass2(), tsName, untHDname, &unt_hd_length);
	void *bg_unt = DVD_GetFile(GetDVDClass2(), tsName, untname);


	ObjLookupEntry *lookups = (ObjLookupEntry*)bg_unt_hd_data;

	int objCount = unt_hd_length / sizeof(ObjLookupEntry);

	for (int i = 0; i < objCount; i++) {
		// process each object
		u8 *thisObj = (u8*)((u32)bg_unt + lookups[i].offset);

		SwapObjData(thisObj, slotNumber);
	}
}


void SwapObjData(u8 *data, int slotNumber) {
	// rudimentary parser which will hopefully work

	while (*data != 0xFF) {
		u8 cmd = *data;

		if (cmd == 0xFE || (cmd & 0x80) != 0) {
			data++;
			continue;
		}

		if ((data[2] & 3) != 0) {
			data[2] &= 0xFC;
			data[2] |= slotNumber;
		}
		data += 3;
	}

}


//
// processed\../src/palaceDude.cpp
//

#include <game.h>
#include <stage.h>
#include "msgbox.h"
#include "boss.h" // for stopbgmmusic and crap

const char *PalaceDudeFileList[] = {"OpeningScene", 0};

class dPalaceDude_c : public dStageActor_c {
	public:
		static dPalaceDude_c *build();

		bool hasBeenActivated;
		bool hasExitedStage;
		int onExecute();
};

/*****************************************************************************/
// Glue Code
dPalaceDude_c *dPalaceDude_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(dPalaceDude_c));
	dPalaceDude_c *c = new(buffer) dPalaceDude_c;
	return c;
}


int dPalaceDude_c::onExecute() {
	if (dFlagMgr_c::instance->flags & spriteFlagMask) {
		if (!hasBeenActivated) {
//			OSReport("Activating Palace Dude\n");
			hasBeenActivated = true;

			int delay;
			if (!(settings & 0xF000000)) {
				delay = 270;
				StopBGMMusic();
				SaveBlock *save = GetSaveFile()->GetBlock(-1);
				GameMgrP->switchPalaceFlag |= (1 << (settings >> 28));
			} else {
				delay = 1020;
			}

			dMsgBoxManager_c::instance->showMessage(
				settings & 0xFFFFFF, // message ID
				false, // cannot cancel
				delay // delay
				);
		}
	}

	if (hasBeenActivated) {
		if (hasExitedStage)
			return true;
//		OSReport("Palace Dude is activated, %d\n", dMsgBoxManager_c::instance->visible);
		if (!dMsgBoxManager_c::instance->visible) {
//			OSReport("Exiting\n");
			u32 wmsettings = 0;
			if (settings & 0xF000000) {
				SaveGame(0, false);
				wmsettings = 0x80000000;
			}
			ExitStage(WORLD_MAP, wmsettings, BEAT_LEVEL, MARIO_WIPE);
			hasExitedStage = true;
		}
	}

	return true;

}



//
// processed\../src/eventblock.cpp
//

#include <common.h>
#include <game.h>

// Patches MIST_INTERMITTENT (sprite 239)

class daEnEventBlock_c : public daEnBlockMain_c {
public:
	enum Mode {
		TOGGLE_EVENT = 0,
		SWAP_EVENTS = 1
	};

	TileRenderer tile;
	Physics::Info physicsInfo;

	u8 event1;
	u8 event2;
	Mode mode;

	void equaliseEvents();

	int onCreate();
	int onDelete();
	int onExecute();

	void calledWhenUpMoveExecutes();
	void calledWhenDownMoveExecutes();

	void blockWasHit(bool isDown);

	USING_STATES(daEnEventBlock_c);
	DECLARE_STATE(Wait);

	static daEnEventBlock_c *build();
};


CREATE_STATE(daEnEventBlock_c, Wait);


int daEnEventBlock_c::onCreate() {
	blockInit(pos.y);

	physicsInfo.x1 = -8;
	physicsInfo.y1 = 16;
	physicsInfo.x2 = 8;
	physicsInfo.y2 = 0;

	physicsInfo.otherCallback1 = &daEnBlockMain_c::OPhysicsCallback1;
	physicsInfo.otherCallback2 = &daEnBlockMain_c::OPhysicsCallback2;
	physicsInfo.otherCallback3 = &daEnBlockMain_c::OPhysicsCallback3;

	physics.setup(this, &physicsInfo, 3, currentLayerID);
	physics.flagsMaybe = 0x260;
	physics.callback1 = &daEnBlockMain_c::PhysicsCallback1;
	physics.callback2 = &daEnBlockMain_c::PhysicsCallback2;
	physics.callback3 = &daEnBlockMain_c::PhysicsCallback3;
	physics.addToList();

	TileRenderer::List *list = dBgGm_c::instance->getTileRendererList(0);
	list->add(&tile);

	tile.x = pos.x - 8;
	tile.y = -(16 + pos.y);
	tile.tileNumber = 0x97;

	mode = (Mode)((settings >> 16) & 0xF);
	event1 = ((settings >> 8) & 0xFF) - 1;
	event2 = (settings & 0xFF) - 1;

	equaliseEvents();

	doStateChange(&daEnEventBlock_c::StateID_Wait);

	return true;
}


int daEnEventBlock_c::onDelete() {
	TileRenderer::List *list = dBgGm_c::instance->getTileRendererList(0);
	list->remove(&tile);

	physics.removeFromList();

	return true;
}


int daEnEventBlock_c::onExecute() {
	acState.execute();
	physics.update();
	blockUpdate();

	tile.setPosition(pos.x-8, -(16+pos.y), pos.z);
	tile.setVars(scale.x);

	equaliseEvents();

	bool isActive = dFlagMgr_c::instance->active(event2);

	tile.tileNumber = (isActive ? 0x96 : 0x97);

	// now check zone bounds based on state
	if (acState.getCurrentState()->isEqual(&StateID_Wait)) {
		checkZoneBoundaries(0);
	}

	return true;
}


daEnEventBlock_c *daEnEventBlock_c::build() {

	void *buffer = AllocFromGameHeap1(sizeof(daEnEventBlock_c));
	daEnEventBlock_c *c = new(buffer) daEnEventBlock_c;


	return c;
}


void daEnEventBlock_c::equaliseEvents() {
	if (mode != SWAP_EVENTS)
		return;

	bool f1 = dFlagMgr_c::instance->active(event1);
	bool f2 = dFlagMgr_c::instance->active(event2);

	if (!f1 && !f2) {
		dFlagMgr_c::instance->set(event1, 0, true, false, false);
	}

	if (f1 && f2) {
		dFlagMgr_c::instance->set(event2, 0, false, false, false);
	}
}


void daEnEventBlock_c::blockWasHit(bool isDown) {
	pos.y = initialY;

	if (mode == TOGGLE_EVENT) {
		if (dFlagMgr_c::instance->active(event2))
			dFlagMgr_c::instance->set(event2, 0, false, false, false);
		else
			dFlagMgr_c::instance->set(event2, 0, true, false, false);

	} else if (mode == SWAP_EVENTS) {
		if (dFlagMgr_c::instance->active(event1)) {
			dFlagMgr_c::instance->set(event1, 0, false, false, false);
			dFlagMgr_c::instance->set(event2, 0, true, false, false);
		} else {
			dFlagMgr_c::instance->set(event1, 0, true, false, false);
			dFlagMgr_c::instance->set(event2, 0, false, false, false);
		}
	}

	physics.setup(this, &physicsInfo, 3, currentLayerID);
	physics.addToList();
	
	doStateChange(&StateID_Wait);
}



void daEnEventBlock_c::calledWhenUpMoveExecutes() {
	if (initialY >= pos.y)
		blockWasHit(false);
}

void daEnEventBlock_c::calledWhenDownMoveExecutes() {
	if (initialY <= pos.y)
		blockWasHit(true);
}



void daEnEventBlock_c::beginState_Wait() {
}

void daEnEventBlock_c::endState_Wait() {
}

void daEnEventBlock_c::executeState_Wait() {
	int result = blockResult();

	if (result == 0)
		return;

	if (result == 1) {
		doStateChange(&daEnBlockMain_c::StateID_UpMove);
		anotherFlag = 2;
		isGroundPound = false;
	} else {
		doStateChange(&daEnBlockMain_c::StateID_DownMove);
		anotherFlag = 1;
		isGroundPound = true;
	}
}



//
// processed\../src/msgbox.cpp
//

#include <common.h>
#include <game.h>
#include "msgbox.h"

// Replaces: EN_LIFT_ROTATION_HALF (Sprite 107; Profile ID 481 @ 80AF96F8)


dMsgBoxManager_c *dMsgBoxManager_c::instance = 0;
dMsgBoxManager_c *dMsgBoxManager_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(dMsgBoxManager_c));
	dMsgBoxManager_c *c = new(buffer) dMsgBoxManager_c;

	instance = c;
	return c;
}

#define ANIM_BOX_APPEAR 0
#define ANIM_BOX_DISAPPEAR 1

extern int MessageBoxIsShowing;

/*****************************************************************************/
// Events
int dMsgBoxManager_c::onCreate() {
	if (!layoutLoaded) {
		if (!layout.loadArc("msgbox.arc", false))
			return false;

		static const char *brlanNames[2] = {
			"BoxAppear.brlan",
			"BoxDisappear.brlan",
		};

		static const char *groupNames[2] = {
			"G_Box", "G_Box",
		};

		layout.build("MessageBox.brlyt");

		if (IsWideScreen()) {
			layout.layout.rootPane->scale.x = 0.7711f;
		}

		layout.loadAnimations(brlanNames, 2);
		layout.loadGroups(groupNames, (int[2]){0,1}, 2);
		layout.disableAllAnimations();

		layout.drawOrder = 140;

		layoutLoaded = true;
	}

	visible = false;

	return true;
}

int dMsgBoxManager_c::onExecute() {
	state.execute();

	layout.execAnimations();
	layout.update();

	return true;
}

int dMsgBoxManager_c::onDraw() {
	if (visible) {
		layout.scheduleForDrawing();
	}
	
	return true;
}

int dMsgBoxManager_c::onDelete() {
	instance = 0;

	MessageBoxIsShowing = false;
	if (canCancel && StageC4::instance)
		StageC4::instance->_1D = 0; // disable no-pause
	msgDataLoader.unload();

	return layout.free();
}

/*****************************************************************************/
// Load Resources
CREATE_STATE_E(dMsgBoxManager_c, LoadRes);

void dMsgBoxManager_c::executeState_LoadRes() {
	if (msgDataLoader.load("/NewerRes/Messages.bin")) {
		state.setState(&StateID_Wait);
	} else {
	}
}

/*****************************************************************************/
// Waiting
CREATE_STATE_E(dMsgBoxManager_c, Wait);

void dMsgBoxManager_c::executeState_Wait() {
	// null
}

/*****************************************************************************/
// Show Box
void dMsgBoxManager_c::showMessage(int id, bool canCancel, int delay) {
	if (!this) {
		OSReport("ADD A MESSAGE BOX MANAGER YOU MORON\n");
		return;
	}

	// get the data file
	header_s *data = (header_s*)msgDataLoader.buffer;

	const wchar_t *title = 0, *msg = 0;

	for (int i = 0; i < data->count; i++) {
		if (data->entry[i].id == id) {
			title = (const wchar_t*)((u32)data + data->entry[i].titleOffset);
			msg = (const wchar_t*)((u32)data + data->entry[i].msgOffset);
			break;
		}
	}

	if (title == 0) {
		OSReport("Message Box: Message %08x not found\n", id);
		return;
	}

	layout.findTextBoxByName("T_title")->SetString(title);
	layout.findTextBoxByName("T_msg")->SetString(msg);

	this->canCancel = canCancel;
	this->delay = delay;
	layout.findPictureByName("button")->SetVisible(canCancel);

	state.setState(&StateID_BoxAppearWait);
}


CREATE_STATE(dMsgBoxManager_c, BoxAppearWait);

void dMsgBoxManager_c::beginState_BoxAppearWait() {
	visible = true;
	MessageBoxIsShowing = true;
	StageC4::instance->_1D = 1; // enable no-pause
	layout.enableNonLoopAnim(ANIM_BOX_APPEAR);

	nw4r::snd::SoundHandle handle;
	PlaySoundWithFunctionB4(SoundRelatedClass, &handle, SE_SYS_KO_DIALOGUE_IN, 1);
}

void dMsgBoxManager_c::executeState_BoxAppearWait() {
	if (!layout.isAnimOn(ANIM_BOX_APPEAR)) {
		state.setState(&StateID_ShownWait);
	}
}

void dMsgBoxManager_c::endState_BoxAppearWait() { }

/*****************************************************************************/
// Wait For Player To Finish
CREATE_STATE(dMsgBoxManager_c, ShownWait);

void dMsgBoxManager_c::beginState_ShownWait() { }
void dMsgBoxManager_c::executeState_ShownWait() {
	if (canCancel) {
		int nowPressed = Remocon_GetPressed(GetActiveRemocon());

		if (nowPressed & WPAD_TWO)
			state.setState(&StateID_BoxDisappearWait);
	}

	if (delay > 0) {
		delay--;
		if (delay == 0)
			state.setState(&StateID_BoxDisappearWait);
	}
}
void dMsgBoxManager_c::endState_ShownWait() { }

/*****************************************************************************/
// Hide Box
CREATE_STATE(dMsgBoxManager_c, BoxDisappearWait);

void dMsgBoxManager_c::beginState_BoxDisappearWait() {
	layout.enableNonLoopAnim(ANIM_BOX_DISAPPEAR);

	nw4r::snd::SoundHandle handle;
	PlaySoundWithFunctionB4(SoundRelatedClass, &handle, SE_SYS_DIALOGUE_OUT_AUTO, 1);
}

void dMsgBoxManager_c::executeState_BoxDisappearWait() {
	if (!layout.isAnimOn(ANIM_BOX_DISAPPEAR)) {
		state.setState(&StateID_Wait);

		for (int i = 0; i < 2; i++)
			layout.resetAnim(i);
		layout.disableAllAnimations();
	}
}

void dMsgBoxManager_c::endState_BoxDisappearWait() {
	visible = false;
	MessageBoxIsShowing = false;
	if (canCancel && StageC4::instance)
		StageC4::instance->_1D = 0; // disable no-pause
}



/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
// Replaces: EN_BLUR (Sprite 152; Profile ID 603 @ 80ADD890)


class daEnMsgBlock_c : public daEnBlockMain_c {
public:
	TileRenderer tile;
	Physics::Info physicsInfo;

	int onCreate();
	int onDelete();
	int onExecute();

	void calledWhenUpMoveExecutes();
	void calledWhenDownMoveExecutes();

	void blockWasHit(bool isDown);

	USING_STATES(daEnMsgBlock_c);
	DECLARE_STATE(Wait);

	static daEnMsgBlock_c *build();
};


CREATE_STATE(daEnMsgBlock_c, Wait);


int daEnMsgBlock_c::onCreate() {
	blockInit(pos.y);

	physicsInfo.x1 = -8;
	physicsInfo.y1 = 16;
	physicsInfo.x2 = 8;
	physicsInfo.y2 = 0;

	physicsInfo.otherCallback1 = &daEnBlockMain_c::OPhysicsCallback1;
	physicsInfo.otherCallback2 = &daEnBlockMain_c::OPhysicsCallback2;
	physicsInfo.otherCallback3 = &daEnBlockMain_c::OPhysicsCallback3;

	physics.setup(this, &physicsInfo, 3, currentLayerID);
	physics.flagsMaybe = 0x260;
	physics.callback1 = &daEnBlockMain_c::PhysicsCallback1;
	physics.callback2 = &daEnBlockMain_c::PhysicsCallback2;
	physics.callback3 = &daEnBlockMain_c::PhysicsCallback3;
	physics.addToList();

	TileRenderer::List *list = dBgGm_c::instance->getTileRendererList(0);
	list->add(&tile);

	tile.x = pos.x - 8;
	tile.y = -(16 + pos.y);
	tile.tileNumber = 0x98;

	doStateChange(&daEnMsgBlock_c::StateID_Wait);

	return true;
}


int daEnMsgBlock_c::onDelete() {
	TileRenderer::List *list = dBgGm_c::instance->getTileRendererList(0);
	list->remove(&tile);

	physics.removeFromList();

	return true;
}


int daEnMsgBlock_c::onExecute() {
	acState.execute();
	physics.update();
	blockUpdate();

	tile.setPosition(pos.x-8, -(16+pos.y), pos.z);
	tile.setVars(scale.x);

	// now check zone bounds based on state
	if (acState.getCurrentState()->isEqual(&StateID_Wait)) {
		checkZoneBoundaries(0);
	}

	return true;
}


daEnMsgBlock_c *daEnMsgBlock_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daEnMsgBlock_c));
	return new(buffer) daEnMsgBlock_c;
}


void daEnMsgBlock_c::blockWasHit(bool isDown) {
	pos.y = initialY;

	if (dMsgBoxManager_c::instance)
		dMsgBoxManager_c::instance->showMessage(settings);
	else
		Delete(false);

	physics.setup(this, &physicsInfo, 3, currentLayerID);
	physics.addToList();
	
	doStateChange(&StateID_Wait);
}



void daEnMsgBlock_c::calledWhenUpMoveExecutes() {
	if (initialY >= pos.y)
		blockWasHit(false);
}

void daEnMsgBlock_c::calledWhenDownMoveExecutes() {
	if (initialY <= pos.y)
		blockWasHit(true);
}



void daEnMsgBlock_c::beginState_Wait() {
}

void daEnMsgBlock_c::endState_Wait() {
}

void daEnMsgBlock_c::executeState_Wait() {
	int result = blockResult();

	if (result == 0)
		return;

	if (result == 1) {
		doStateChange(&daEnBlockMain_c::StateID_UpMove);
		anotherFlag = 2;
		isGroundPound = false;
	} else {
		doStateChange(&daEnBlockMain_c::StateID_DownMove);
		anotherFlag = 1;
		isGroundPound = true;
	}
}



//
// processed\../src/eventlooper.cpp
//

#include <common.h>
#include <game.h>

struct EventLooper {
	u32 id;			// 0x00
	u32 settings;	// 0x04
	u16 name;		// 0x08
	u8 _0A[6];		// 0x0A
	u8 _10[0x9C];	// 0x10
	float x;		// 0xAC
	float y;		// 0xB0
	float z;		// 0xB4
	u8 _B8[0x318];	// 0xB8
	// Any variables you add to the class go here; starting at offset 0x3D0
	u64 eventFlag;	// 0x3D0
	u64 eventActive;	// 0x3D0
	u8 delay;		// 0x3D4
	u8 delayCount;	// 0x3D7
};

void EventLooper_Update(EventLooper *self);



bool EventLooper_Create(EventLooper *self) {
	char eventStart	= (self->settings >> 24)	& 0xFF;
	char eventEnd	= (self->settings >> 16)	& 0xFF;

	// Putting all the events into the flag
	int i;
	u64 q = (u64)0;
	for(i=eventStart;i<(eventEnd+1);i++)
	{
		q = q | ((u64)1 << (i - 1));
	}
		
	self->eventFlag = q;
	
	self->delay		= (((self->settings) & 0xFF) + 1) * 10;
	self->delayCount = 0;
	
	char tmpEvent= (self->settings >> 8)	& 0xFF;
	if (tmpEvent == 0)
	{
		self->eventActive = (u64)0xFFFFFFFFFFFFFFFF;
	}
	else
	{
		self->eventActive = (u64)1 << (tmpEvent - 1);
		
	}
	

	if (dFlagMgr_c::instance->flags & self->eventActive)
	{
		u64 evState = (u64)1 << (eventStart - 1);
		dFlagMgr_c::instance->flags |= evState;
	}

	EventLooper_Update(self);
	
	return true;
}

bool EventLooper_Execute(EventLooper *self) {
	EventLooper_Update(self);
	return true;
}


void EventLooper_Update(EventLooper *self) {
	
	if ((dFlagMgr_c::instance->flags & self->eventActive) == 0)
		return;

	// Waiting for the right moment
	if (self->delayCount < self->delay) 
	{

		self->delayCount = self->delayCount + 1;
		return;
	}	
	
	// Reset the delay
	self->delayCount = 0;
	
	// Find which event(s) is/are on
	u64 evState = dFlagMgr_c::instance->flags & self->eventFlag;
	
	// Turn off the old events
	dFlagMgr_c::instance->flags = dFlagMgr_c::instance->flags & (~self->eventFlag);
	
	// Shift them right if they can, if not, reset!
	evState = evState << 1;
	if (evState < self->eventFlag)
	{
		dFlagMgr_c::instance->flags = dFlagMgr_c::instance->flags | evState;
	}
	
	else
	{
		char eventStart	= (self->settings >> 24)	& 0xFF;
		evState = (u64)1 << (eventStart - 1);
		dFlagMgr_c::instance->flags = dFlagMgr_c::instance->flags | evState;
	}
	
	
}

//
// processed\../src/spritespawner.cpp
//

#include <game.h>

class dSpriteSpawner_c : public dStageActor_c {
	public:
		static dSpriteSpawner_c *build();

		u64 classicEventOverride;
		Actors profileID;
		bool respawn;
		u32 childSettings;
		u32 childID;

		int onCreate();
		int onExecute();
};

/*****************************************************************************/
// Glue Code
dSpriteSpawner_c *dSpriteSpawner_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(dSpriteSpawner_c));
	dSpriteSpawner_c *c = new(buffer) dSpriteSpawner_c;
	return c;
}


int dSpriteSpawner_c::onCreate() {
	char classicEventNum = (settings >> 28) & 0xF;
	classicEventOverride = (classicEventNum == 0) ? 0 : ((u64)1 << (classicEventNum - 1));

	profileID = translateActorID( (Actors)((settings >> 16) & 0x7FF) );
	respawn = (settings >> 27) & 1;

	u16 tempSet = settings & 0xFFFF;
	childSettings =
		(tempSet & 3) | ((tempSet & 0xC) << 2) |
		((tempSet & 0x30) << 4) | ((tempSet & 0xC0) << 6) |
		((tempSet & 0x300) << 8) | ((tempSet & 0xC00) << 10) |
		((tempSet & 0x3000) << 12) | ((tempSet & 0xC000) << 14);

	return true;
}


int dSpriteSpawner_c::onExecute() {
	u64 effectiveFlag = classicEventOverride | spriteFlagMask;

	if (dFlagMgr_c::instance->flags & effectiveFlag) {
		if (!childID) {
			dStageActor_c *newAc = dStageActor_c::create(profileID, childSettings, &pos, 0, 0);
			childID = newAc->id;
		}
	} else {
		if (respawn)
			return true;

		if (childID) {
			dStageActor_c *ac = (dStageActor_c*)fBase_c::search(childID);
			if (ac) {
				pos = ac->pos;
				ac->Delete(1);
			}
			childID = 0;
		}
	}

	if (respawn) {
		if (childID) {
			dStageActor_c *ac = (dStageActor_c*)fBase_c::search(childID);

			if (!ac) {
				dStageActor_c *newAc = dStageActor_c::create(profileID, childSettings, &pos, 0, 0);
				childID = newAc->id;
			}
		}
	}

	return true;

}


//
// processed\../src/spriteswapper.cpp
//

#include <common.h>
#include <game.h>


class SpriteSpawnerTimed : public dStageActor_c {
public:
	int onCreate();
	int onExecute();

	static SpriteSpawnerTimed *build();

	u64 eventFlag;	// 0x3D0
	u16 type;		// 0x3D4
	u32 inheritSet;	// 0x3D6
	u8 lastEvState;	// 0x3DA
	u32 timer;
};


SpriteSpawnerTimed *SpriteSpawnerTimed::build() {
	void *buffer = AllocFromGameHeap1(sizeof(SpriteSpawnerTimed));
	return new(buffer) SpriteSpawnerTimed;
}



int SpriteSpawnerTimed::onCreate() {

	char eventNum	= (this->settings >> 28)	& 0xF;

	this->eventFlag = (u64)1 << (eventNum - 1);
	this->type		= (this->settings >> 16) & 0xFFF;
	
	short tempSet = this->settings & 0xFFFF;
	this->inheritSet = (tempSet & 3) | ((tempSet & 0xC) << 2) | ((tempSet & 0x30) << 4) | ((tempSet & 0xC0) << 6) | ((tempSet & 0x300) << 8) | ((tempSet & 0xC00) << 10) | ((tempSet & 0x3000) << 12) | ((tempSet & 0xC000) << 14);
	
	this->timer = 0;
	
	return true;
}

int SpriteSpawnerTimed::onExecute() {

	if (dFlagMgr_c::instance->flags & this->eventFlag) {		 // If the event is on
		if (this->timer < 1) {						// If the timer is empty
			CreateActor(this->type, this->inheritSet, this->pos, 0, 0);
			this->timer = 120;
		}		

		this->timer--;
	}

	else {
		this->timer = 0;
	}

	return true;
}

//
// processed\../src/topman.cpp
//

#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>

const char* TMarcNameList [] = {
	"topman",
	NULL	
};

class daTopman : public dEn_c {
	int onCreate();
	int onDelete();
	int onExecute();
	int onDraw();

	mHeapAllocator_c allocator;
	nw4r::g3d::ResFile resFile;

	m3d::mdl_c bodyModel;

	m3d::anmChr_c chrAnimation;

	int timer;
	char damage;
	char isDown;
	float XSpeed;
	u32 cmgr_returnValue;
	bool isBouncing;
	char isInSpace;
	char fromBehind;
	char isWaiting;
	char backFire;
	int directionStore;

	static daTopman *build();

	void bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate);
	void updateModelMatrices();
	bool calculateTileCollisions();

	void playerCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	void yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther);

	bool collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther);
	// bool collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther);

	void _vf148();
	void _vf14C();
	bool CreateIceActors();
	void addScoreWhenHit(void *other);

	USING_STATES(daTopman);
	DECLARE_STATE(Walk);
	DECLARE_STATE(Turn);
	DECLARE_STATE(Wait);
	DECLARE_STATE(KnockBack);
	DECLARE_STATE(Die);
};

daTopman *daTopman::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daTopman));
	return new(buffer) daTopman;
}

///////////////////////
// Externs and States
///////////////////////
	extern "C" void *EN_LandbarrelPlayerCollision(dEn_c* t, ActivePhysics *apThis, ActivePhysics *apOther);

	//FIXME make this dEn_c->used...
	extern "C" char usedForDeterminingStatePress_or_playerCollision(dEn_c* t, ActivePhysics *apThis, ActivePhysics *apOther, int unk1);
	extern "C" int SmoothRotation(short* rot, u16 amt, int unk2);


	CREATE_STATE(daTopman, Walk);
	CREATE_STATE(daTopman, Turn);
	CREATE_STATE(daTopman, Wait);
	CREATE_STATE(daTopman, KnockBack);
	CREATE_STATE(daTopman, Die);

	// 	begoman_attack2"	// wobble back and forth tilted forwards
	// 	begoman_attack3"	// Leaned forward, antennae extended
	// 	begoman_damage"		// Bounces back slightly
	// 	begoman_damage2"	// Stops spinning and wobbles to the ground like a top
	// 	begoman_stand"		// Stands still, waiting
	// 	begoman_wait"		// Dizzily Wobbles
	// 	begoman_wait2"		// spins around just slightly
	// 	begoman_attack"		// Rocks backwards, and then attacks to an upright position, pulsing out his antennae


////////////////////////
// Collision Functions
////////////////////////

	// Collision callback to help shy guy not die at inappropriate times and ruin the dinner

	void daTopman::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther) {

		char hitType;
		hitType = usedForDeterminingStatePress_or_playerCollision(this, apThis, apOther, 0);

		if(hitType == 1 || hitType == 2) {	// regular jump or mini jump
			this->_vf220(apOther->owner);
		} 
		else if(hitType == 3) {	// spinning jump or whatever?
			this->_vf220(apOther->owner);
		} 
		else if(hitType == 0) {
			EN_LandbarrelPlayerCollision(this, apThis, apOther);
			if (this->pos.x > apOther->owner->pos.x) {
				this->backFire = 1;
			}
			else {
				this->backFire = 0;
			}
			doStateChange(&StateID_KnockBack);
		} 

		// fix multiple player collisions via megazig
		deathInfo.isDead = 0;
		this->flags_4FC |= (1<<(31-7));
		this->counter_504[apOther->owner->which_player] = 0;
	}

	void daTopman::yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
		this->playerCollision(apThis, apOther);
	}

	bool daTopman::collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther) {
		this->dEn_c::playerCollision(apThis, apOther);
		this->_vf220(apOther->owner);

		deathInfo.isDead = 0;
		this->flags_4FC |= (1<<(31-7));
		this->counter_504[apOther->owner->which_player] = 0;
		return true;
	}

	bool daTopman::collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther) {
		this->collisionCatD_Drill(apThis, apOther);
		return true;
	}

	bool daTopman::collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther) {
		this->collisionCatD_Drill(apThis, apOther);
		return true;
	}

	bool daTopman::collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther) {
		backFire = apOther->owner->direction ^ 1;
		// doStateChange(&StateID_KnockBack);
		doStateChange(&StateID_Die);
		return true;
	}

	bool daTopman::collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther){
		doStateChange(&StateID_Die);
		return true;
	}

	bool daTopman::collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther) {
		doStateChange(&StateID_Die);
		return true;
	}

	bool daTopman::collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther){
		backFire = apOther->owner->direction ^ 1;
		doStateChange(&StateID_KnockBack);
		return true;
	}

	bool daTopman::collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther){
		backFire = apOther->owner->direction ^ 1;
		doStateChange(&StateID_KnockBack);
		return true;
	}

	bool daTopman::collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther) {
		backFire = apOther->owner->direction ^ 1;
		doStateChange(&StateID_KnockBack);
		return true;
	}

	// void daTopman::collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther) {
	// 	doStateChange(&StateID_DieFall);
	// }

	// These handle the ice crap
	void daTopman::_vf148() {
		dEn_c::_vf148();
		doStateChange(&StateID_Die);
	}
	void daTopman::_vf14C() {
		dEn_c::_vf14C();
		doStateChange(&StateID_Die);
	}

	DoSomethingCool my_struct;

	extern "C" void sub_80024C20(void);
	extern "C" void __destroy_arr(void*, void(*)(void), int, int);
	//extern "C" __destroy_arr(struct DoSomethingCool, void(*)(void), int cnt, int bar);

	bool daTopman::CreateIceActors()
	{
	    struct DoSomethingCool my_struct = { 0, this->pos, {2.5, 2.5, 2.5}, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
	    this->frzMgr.Create_ICEACTORs( (void*)&my_struct, 1 );
	    __destroy_arr( (void*)&my_struct, sub_80024C20, 0x3C, 1 );
	    return true;
	}

	void daTopman::addScoreWhenHit(void *other) {}


bool daTopman::calculateTileCollisions() {
	// Returns true if sprite should turn, false if not.

	HandleXSpeed();
	HandleYSpeed();
	doSpriteMovement();

	cmgr_returnValue = collMgr.isOnTopOfTile();
	collMgr.calculateBelowCollisionWithSmokeEffect();

	if (isBouncing) {
		stuffRelatingToCollisions(0.1875f, 1.0f, 0.5f);
		if (speed.y != 0.0f)
			isBouncing = false;
	}

	float xDelta = pos.x - last_pos.x;
	if (xDelta >= 0.0f)
		direction = 0;
	else
		direction = 1;

	if (collMgr.isOnTopOfTile()) {
		// Walking into a tile branch

		if (cmgr_returnValue == 0)
			isBouncing = true;

		if (speed.x != 0.0f) {
			//playWmEnIronEffect();
		}

		speed.y = 0.0f;

		// u32 blah = collMgr.s_80070760();
		// u8 one = (blah & 0xFF);
		// static const float incs[5] = {0.00390625f, 0.0078125f, 0.015625f, 0.0234375f, 0.03125f};
		// x_speed_inc = incs[one];
		max_speed.x = (direction == 1) ? -0.8f : 0.8f;
	} else {
		x_speed_inc = 0.0f;
	}

	// Bouncing checks
	if (_34A & 4) {
		Vec v = (Vec){0.0f, 1.0f, 0.0f};
		collMgr.pSpeed = &v;

		if (collMgr.calculateAboveCollision(collMgr.outputMaybe))
			speed.y = 0.0f;

		collMgr.pSpeed = &speed;

	} else {
		if (collMgr.calculateAboveCollision(collMgr.outputMaybe))
			speed.y = 0.0f;
	}

	collMgr.calculateAdjacentCollision(0);

	// Switch Direction
	if (collMgr.outputMaybe & (0x15 << direction)) {
		if (collMgr.isOnTopOfTile()) {
			isBouncing = true;
		}
		return true;
	}
	return false;
}

void daTopman::bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate) {
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr(name);
	this->chrAnimation.bind(&this->bodyModel, anmChr, unk);
	this->bodyModel.bindAnim(&this->chrAnimation, unk2);
	this->chrAnimation.setUpdateRate(rate);
}

int daTopman::onCreate() {

	this->deleteForever = true;
	
	// Model creation	
	allocator.link(-1, GameHeaps[0], 0, 0x20);

	this->resFile.data = getResource("topman", "g3d/begoman_spike.brres");
	nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("begoman");
	bodyModel.setup(mdl, &allocator, 0x224, 1, 0);
	SetupTextures_Map(&bodyModel, 0);


	// Animations start here
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr("begoman_wait");
	this->chrAnimation.setup(mdl, anmChr, &this->allocator, 0);

	allocator.unlink();

	// Stuff I do understand
	this->scale = (Vec){0.2, 0.2, 0.2};

	// this->pos.y = this->pos.y + 30.0; // X is vertical axis
	this->rot.x = 0; // X is vertical axis
	this->rot.y = 0xD800; // Y is horizontal axis
	this->rot.z = 0; // Z is ... an axis >.>
	this->direction = 1; // Heading left.
	
	this->speed.x = 0.0;
	this->speed.y = 0.0;
	this->max_speed.x = 0.8;
	this->x_speed_inc = 0.0;
	this->XSpeed = 0.8;

	this->isInSpace = this->settings & 0xF;
	this->isWaiting = (this->settings >> 4) & 0xF;
	this->fromBehind = 0;

	ActivePhysics::Info HitMeBaby;

	HitMeBaby.xDistToCenter = 0.0;
	HitMeBaby.yDistToCenter = 12.0;

	HitMeBaby.xDistToEdge = 14.0;
	HitMeBaby.yDistToEdge = 12.0;		

	HitMeBaby.category1 = 0x3;
	HitMeBaby.category2 = 0x0;
	HitMeBaby.bitfield1 = 0x4F;
	HitMeBaby.bitfield2 = 0xffbafffe;
	HitMeBaby.unkShort1C = 0;
	HitMeBaby.callback = &dEn_c::collisionCallback;

	this->aPhysics.initWithStruct(this, &HitMeBaby);
	this->aPhysics.addToList();


	// Tile collider

	// These fucking rects do something for the tile rect
	spriteSomeRectX = 28.0f;
	spriteSomeRectY = 32.0f;
	_320 = 0.0f;
	_324 = 16.0f;

	static const lineSensor_s below(12<<12, 4<<12, 0<<12);
	static const pointSensor_s above(0<<12, 12<<12);
	static const lineSensor_s adjacent(6<<12, 9<<12, 14<<12);

	collMgr.init(this, &below, &above, &adjacent);
	collMgr.calculateBelowCollisionWithSmokeEffect();

	cmgr_returnValue = collMgr.isOnTopOfTile();

	if (collMgr.isOnTopOfTile())
		isBouncing = false;
	else
		isBouncing = true;


	// State Changers
	bindAnimChr_and_setUpdateRate("begoman_wait2", 1, 0.0, 1.0); 
	if (this->isWaiting == 0) {
		doStateChange(&StateID_Walk); }
	else {
		doStateChange(&StateID_Wait); }

	this->onExecute();
	return true;
}

int daTopman::onDelete() {
	return true;
}

int daTopman::onExecute() {
	acState.execute();
	updateModelMatrices();
	
	float rect[] = {0.0, 0.0, 38.0, 38.0};
	int ret = this->outOfZone(this->pos, (float*)&rect, this->currentZoneID);
	if(ret) {
		this->Delete(1);
	}
	return true;
}

int daTopman::onDraw() {
	bodyModel.scheduleForDrawing();

	return true;
}

void daTopman::updateModelMatrices() {
	matrix.translation(pos.x, pos.y - 2.0, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	bodyModel.setDrawMatrix(matrix);
	bodyModel.setScale(&scale);
	bodyModel.calcWorld(false);
}


///////////////
// Walk State
///////////////
	void daTopman::beginState_Walk() {
		this->max_speed.x = (this->direction) ? -this->XSpeed : this->XSpeed;
		this->speed.x = (direction) ? -0.8f : 0.8f;

		this->max_speed.y = (this->isInSpace) ? -2.0 : -4.0;
		this->speed.y = 	(this->isInSpace) ? -2.0 : -4.0;
		this->y_speed_inc = (this->isInSpace) ? -0.09375 : -0.1875;
	}
	void daTopman::executeState_Walk() { 

		if (!this->isOutOfView()) {
			nw4r::snd::SoundHandle *handle = PlaySound(this, SE_BOSS_JR_CROWN_JR_RIDE);
			if (handle)
				handle->SetVolume(0.5f, 0); 
		}
	
		bool ret = calculateTileCollisions();
		if (ret) {
			doStateChange(&StateID_Turn);
		}
		bodyModel._vf1C();

		if(this->chrAnimation.isAnimationDone()) {
			this->chrAnimation.setCurrentFrame(0.0);
		}
	}
	void daTopman::endState_Walk() { this->timer += 1; }


///////////////
// Turn State
///////////////
	void daTopman::beginState_Turn() {
		this->direction ^= 1;
		this->speed.x = 0.0;
	}
	void daTopman::executeState_Turn() { 

		bodyModel._vf1C();
		if(this->chrAnimation.isAnimationDone()) {
			this->chrAnimation.setCurrentFrame(0.0);
		}

		u16 amt = (this->direction == 0) ? 0x2800 : 0xD800;
		int done = SmoothRotation(&this->rot.y, amt, 0x800);

		if(done) {
			this->doStateChange(&StateID_Walk);
		}
	}
	void daTopman::endState_Turn() { }


///////////////
// Wait State
///////////////
	void daTopman::beginState_Wait() {
		this->max_speed.x = 0;
		this->speed.x = 0;

		this->max_speed.y = (this->isInSpace) ? -2.0 : -4.0;
		this->speed.y = 	(this->isInSpace) ? -2.0 : -4.0;
		this->y_speed_inc = (this->isInSpace) ? -0.09375 : -0.1875;
	}
	void daTopman::executeState_Wait() { 

		if (!this->isOutOfView()) {
			nw4r::snd::SoundHandle *handle = PlaySound(this, SE_BOSS_JR_CROWN_JR_RIDE);
			if (handle)
				handle->SetVolume(0.5f, 0); 
		}
	
		bool ret = calculateTileCollisions();
		if (ret) {
			doStateChange(&StateID_Turn);
		}

		bodyModel._vf1C();
		if(this->chrAnimation.isAnimationDone()) {
			this->chrAnimation.setCurrentFrame(0.0);
		}
	}
	void daTopman::endState_Wait() { }


///////////////
// Die State
///////////////
	void daTopman::beginState_Die() {
		dEn_c::dieFall_Begin();

		bindAnimChr_and_setUpdateRate("begoman_damage2", 1, 0.0, 1.0); 
		this->timer = 0;
	}
	void daTopman::executeState_Die() { 

		bodyModel._vf1C();

		PlaySound(this, SE_EMY_MECHAKOOPA_DAMAGE);
		if(this->chrAnimation.isAnimationDone()) {
			this->kill();
			this->Delete(this->deleteForever);
		}
	}
	void daTopman::endState_Die() { }


///////////////
// Knockback State
///////////////
	void daTopman::beginState_KnockBack() {
		bindAnimChr_and_setUpdateRate("begoman_damage", 1, 0.0, 0.75); 

		directionStore = direction;
		speed.x = (backFire) ? XSpeed*5.0f : XSpeed*-5.0f;
		max_speed.x = speed.x;
	}
	void daTopman::executeState_KnockBack() { 

		bool ret = calculateTileCollisions();
		this->speed.x = this->speed.x / 1.1;

		bodyModel._vf1C();
		if(this->chrAnimation.isAnimationDone()) {
			if (this->isWaiting == 0) {
				OSReport("Done being knocked back, going back to Walk state\n");
				doStateChange(&StateID_Walk); }
			else {
				OSReport("Done being knocked back, going back to Wait state\n");
				doStateChange(&StateID_Wait); }
		}

	}
	void daTopman::endState_KnockBack() { 
		direction = directionStore;
		bindAnimChr_and_setUpdateRate("begoman_wait2", 1, 0.0, 1.0); 
	}
	

//
// processed\../src/bossMegaGoomba.cpp
//

#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include "boss.h"

extern "C" void *StageScreen;

const char* MGarcNameList [] = {
	"kuriboBig",
	"kuriboBoss",
	NULL	
};

class daMegaGoomba_c : public dEn_c {
	public:
	int onCreate();
	int onDelete();
	int onExecute();
	int onDraw();

	mHeapAllocator_c allocator;
	nw4r::g3d::ResFile resFile;
	m3d::mdl_c bodyModel;
	m3d::anmChr_c animationChr;

	float timer;
	float dying;

	lineSensor_s belowSensor;
	lineSensor_s adjacentSensor;

	ActivePhysics leftTrapAPhysics, rightTrapAPhysics;
	ActivePhysics stalkAPhysics;

	HermiteKey keysX[0x10];
	unsigned int Xkey_count;
	HermiteKey keysY[0x10];
	unsigned int Ykey_count;

	char life;
	bool already_hit;

	float XSpeed;
	float JumpHeight;
	float JumpDist;
	float JumpTime;

	char isBigBoss;
	char isPanic;

	bool takeHit(char count);

	void bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate);
	
	void dieFall_Begin();
	void dieFall_Execute();
	static daMegaGoomba_c *build();

	void setupBodyModel();
	void setupCollision();

	void updateModelMatrices();

	void stunPlayers();
	void unstunPlayers();

	bool hackOfTheCentury;

	bool playerStunned[4];

	void removeMyActivePhysics();
	void addMyActivePhysics();

	int tryHandleJumpedOn(ActivePhysics *apThis, ActivePhysics *apOther);

	void spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	void playerCollision(ActivePhysics *apThis, ActivePhysics *apOther);

	bool collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat11_PipeCannon(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther);
	void addScoreWhenHit(void *other);
	bool _vf120(ActivePhysics *apThis, ActivePhysics *apOther);
	bool _vf110(ActivePhysics *apThis, ActivePhysics *apOther);
	bool _vf108(ActivePhysics *apThis, ActivePhysics *apOther);

	void powBlockActivated(bool isNotMPGP);

	void dieOther_Begin();
	void dieOther_Execute();
	void dieOther_End();

	USING_STATES(daMegaGoomba_c);
	DECLARE_STATE(Shrink);
	DECLARE_STATE(Walk);
	DECLARE_STATE(Turn);
};


daMegaGoomba_c *daMegaGoomba_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daMegaGoomba_c));
	return new(buffer) daMegaGoomba_c;
}


void daMegaGoomba_c::removeMyActivePhysics() {
	aPhysics.removeFromList();
	stalkAPhysics.removeFromList();
	leftTrapAPhysics.removeFromList();
	rightTrapAPhysics.removeFromList();
}

void daMegaGoomba_c::addMyActivePhysics() {
	aPhysics.addToList();
	stalkAPhysics.addToList();
	leftTrapAPhysics.addToList();
	rightTrapAPhysics.addToList();
}


void setNewActivePhysicsRect(daMegaGoomba_c *actor, Vec *scale) {
	float amtX = scale->x * 0.5f;
	float amtY = scale->y * 0.5f;

	actor->belowSensor.flags = SENSOR_LINE;
	actor->belowSensor.lineA = s32((amtX * -28.0f) * 4096.0f);
	actor->belowSensor.lineB = s32((amtX * 28.0f) * 4096.0f);
	actor->belowSensor.distanceFromCenter = 0;

	actor->adjacentSensor.flags = SENSOR_LINE;
	actor->adjacentSensor.lineA = s32((amtY * 4.0f) * 4096.0f);
	actor->adjacentSensor.lineB = s32((amtY * 32.0f) * 4096.0f);
	actor->adjacentSensor.distanceFromCenter = s32((amtX * 46.0f) * 4096.0f);

	u8 cat1 = 3, cat2 = 0;
	u32 bitfield1 = 0x6f, bitfield2 = 0xffbafffe;

	ActivePhysics::Info info = {
		0.0f, amtY*57.0f, amtX*20.0f, amtY*31.0f,
		cat1, cat2, bitfield1, bitfield2, 0, &dEn_c::collisionCallback};
	actor->aPhysics.initWithStruct(actor, &info);

	// Original trapezium was -12,12 to -48,48
	ActivePhysics::Info left = {
		amtX*-32.0f, amtY*55.0f, amtX*12.0f, amtY*30.0f,
		cat1, cat2, bitfield1, bitfield2, 0, &dEn_c::collisionCallback};
	actor->leftTrapAPhysics.initWithStruct(actor, &left);
	actor->leftTrapAPhysics.trpValue0 = amtX * 12.0f;
	actor->leftTrapAPhysics.trpValue1 = amtX * 12.0f;
	actor->leftTrapAPhysics.trpValue2 = amtX * -12.0f;
	actor->leftTrapAPhysics.trpValue3 = amtX * 12.0f;
	actor->leftTrapAPhysics.collisionCheckType = 3;

	ActivePhysics::Info right = {
		amtX*32.0f, amtY*55.0f, amtX*12.0f, amtY*30.0f,
		cat1, cat2, bitfield1, bitfield2, 0, &dEn_c::collisionCallback};
	actor->rightTrapAPhysics.initWithStruct(actor, &right);
	actor->rightTrapAPhysics.trpValue0 = amtX * -12.0f;
	actor->rightTrapAPhysics.trpValue1 = amtX * -12.0f;
	actor->rightTrapAPhysics.trpValue2 = amtX * -12.0f;
	actor->rightTrapAPhysics.trpValue3 = amtX * 12.0f;
	actor->rightTrapAPhysics.collisionCheckType = 3;

	ActivePhysics::Info stalk = {
		0.0f, amtY*12.0f, amtX*28.0f, amtY*12.0f,
		cat1, cat2, bitfield1, bitfield2, 0, &dEn_c::collisionCallback};
	actor->stalkAPhysics.initWithStruct(actor, &stalk);

}


//FIXME make this dEn_c->used...
extern "C" int SomeStrangeModification(dStageActor_c* actor);
extern "C" void DoStuffAndMarkDead(dStageActor_c *actor, Vec vector, float unk);
extern "C" int SmoothRotation(short* rot, u16 amt, int unk2);

void daMegaGoomba_c::powBlockActivated(bool isNotMPGP) {
}

CREATE_STATE(daMegaGoomba_c, Shrink);
CREATE_STATE(daMegaGoomba_c, Walk);
CREATE_STATE(daMegaGoomba_c, Turn);


//TODO better fix for possible bug with sign (ex. life=120; count=-9;)
bool daMegaGoomba_c::takeHit(char count) {
	OSReport("Taking a hit!\n");
	if(!this->already_hit) {
		int c = count;
		int l = this->life;
		if(l - c > 127) {
			c = 127 - l;
		}
		this->life -= c;
		// this->XSpeed += 0.10;

		// float rate = this->animationChr.getUpdateRate();
		// this->animationChr.setUpdateRate(rate+0.05);
		this->JumpHeight += 12.0;
		this->JumpDist += 12.0;
		this->JumpTime += 5.0;
		doStateChange(&StateID_Shrink);
		this->already_hit = true;
	}
	return (life <= 0) ? true : false;
}

#define ACTIVATE	1
#define DEACTIVATE	0

extern "C" void *EN_LandbarrelPlayerCollision(dEn_c* t, ActivePhysics *apThis, ActivePhysics *apOther);
void daMegaGoomba_c::spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
	//HE'S TOO BADASS TO STOP FOR SMALLER GOOMBAS
	#if 0
		float me = apThis->firstFloatArray[3];
		if(((this->direction == 1) && (me > 0.0)) || ((this->direction == 0) && (me < 0.0))) {
			dStateBase_c* state = this->acState.getCurrentState();
			if(!state->isEqual(&StateID_Turn)) {
				doStateChange(&StateID_Turn);
			}
		}
	#endif
}
void daMegaGoomba_c::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther) { 
	if (apThis == &stalkAPhysics) {
		dEn_c::playerCollision(apThis, apOther);
		return;
	}

	/* * * * * * * * * * * * * * * * * * * * *
	 * 0=normal??,1=dontHit,2=dontKill
	 * daEnBrosBase_c ::player = 0
	 * daEnBrosBase_c::yoshi   = 0
	 * daEnPipePirahna::player = 1
	 * daEnPipePirahna::yoshi  = 1
	 * daEnKuriboBase_c::player = 0
	 * daEnKuriboBase_c::yoshi  = 0
	 * daEnLargeKuribo_c::player = 0
	 * daEnLargeKuribo_c::yoshi  = 2
	 * daEnNokonoko_c::player = 0
	 * daEnNokonoko_c::yoshi  = 0
	 * daEnSubBoss_c     = 2
	 *
	 * * * * * * * * * * * * * * * * * * * * */
	//FIXME rename and make part of dStageActor_c
	//unk=0 does _vfs, unk=1 does playSeCmnStep
	//char ret = usedForDeterminingStatePress_or_playerCollision(this, apThis, apOther, 0);

	if (tryHandleJumpedOn(apThis, apOther) == 0) {
		this->dEn_c::playerCollision(apThis, apOther);
		this->_vf220(apOther->owner);
		this->counter_504[apOther->owner->which_player] = 180;
	}
}

int daMegaGoomba_c::tryHandleJumpedOn(ActivePhysics *apThis, ActivePhysics *apOther) {
	float saveBounce = EnemyBounceValue;
	EnemyBounceValue = 5.2f;

	char ret = usedForDeterminingStatePress_or_playerCollision(this, apThis, apOther, 2);

	EnemyBounceValue = saveBounce;

	if(ret == 1 || ret == 3) {
		apOther->someFlagByte |= 2;
		if(this->takeHit(1)) {
			// kill me
			VEC2 eSpeed = {speed.x, speed.y};
			killWithSpecifiedState(apOther->owner, &eSpeed, &dEn_c::StateID_DieOther);
		}
	}

	return ret;
}
bool daMegaGoomba_c::collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther) { 
	if (this->counter_504[apOther->owner->which_player] > 0) { return false; }
	VEC2 eSpeed = {speed.x, speed.y};
	killWithSpecifiedState(apOther->owner, &eSpeed, &dEn_c::StateID_DieOther);
	return true;
}
bool daMegaGoomba_c::collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther) { 
	return collisionCat7_GroundPound(apThis, apOther);
}

bool daMegaGoomba_c::collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther) {
	return false;
}
bool daMegaGoomba_c::collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther) {
	return false;
}
extern "C" void dAcPy_vf3F8(void* player, dEn_c* monster, int t);
bool daMegaGoomba_c::collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther) {
	if (tryHandleJumpedOn(apThis, apOther) == 0) {
		dAcPy_vf3F8(apOther->owner, this, 3);
		this->counter_504[apOther->owner->which_player] = 0xA;
	}
	return true;
}
bool daMegaGoomba_c::collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther) {
	if(this->takeHit(1))
		doStateChange(&StateID_DieFall);
	return true;
}
bool daMegaGoomba_c::collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther) {
	if(this->takeHit(1))
		doStateChange(&StateID_DieFall);
	return true;
}
bool daMegaGoomba_c::collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther) {
	return collisionCat7_GroundPound(apThis, apOther);
}
bool daMegaGoomba_c::collisionCat11_PipeCannon(ActivePhysics *apThis, ActivePhysics *apOther) {
	return true;
}
bool daMegaGoomba_c::collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther) {
	return true;
}
bool daMegaGoomba_c::collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther) {
	if(this->takeHit(1))
		doStateChange(&StateID_DieFall);
	return true;
}
void daMegaGoomba_c::addScoreWhenHit(void *other) {}
bool daMegaGoomba_c::_vf120(ActivePhysics *apThis, ActivePhysics *apOther) {
	return true; // Replicate existing broken behaviour
}
bool daMegaGoomba_c::_vf110(ActivePhysics *apThis, ActivePhysics *apOther) {
	return true; // Replicate existing broken behaviour
}
bool daMegaGoomba_c::_vf108(ActivePhysics *apThis, ActivePhysics *apOther) {
	return true; // Replicate existing broken behaviour
}

void daMegaGoomba_c::bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate) {
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr(name);
	this->animationChr.bind(&this->bodyModel, anmChr, unk);
	this->bodyModel.bindAnim(&this->animationChr, unk2);
	this->animationChr.setUpdateRate(rate);
}

void daMegaGoomba_c::dieFall_Begin() {
	this->dEn_c::dieFall_Begin();
	PlaySound(this, SE_EMY_KURIBO_L_DAMAGE_03);
}
void daMegaGoomba_c::dieFall_Execute() {
	
	this->timer = this->timer + 1.0;
	
	this->dying = this->dying + 0.15;
	
	this->pos.x = this->pos.x + 0.15;
	this->pos.y = this->pos.y + ((-0.2 * (this->dying*this->dying)) + 5);
	
	this->dEn_c::dieFall_Execute();
}

void daMegaGoomba_c::setupBodyModel() {
	allocator.link(-1, GameHeaps[0], 0, 0x20);

	this->resFile.data = getResource("kuriboBoss", "g3d/kuriboBoss.brres");
	nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("kuriboBig");
	bodyModel.setup(mdl, &allocator, 0x224, 1, 0);
	SetupTextures_Enemy(&bodyModel, 0);

	bool ret;
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr("walk");
	ret = this->animationChr.setup(mdl, anmChr, &this->allocator, 0);
	this->bindAnimChr_and_setUpdateRate("walk", 1, 0.0, 0.2);

	allocator.unlink();
}

void daMegaGoomba_c::setupCollision() {
	//POINTLESS WITH GROWTH
	this->scale.x = this->scale.y = this->scale.z = 0.666;

	this->collMgr.init(this, &belowSensor, 0, &adjacentSensor);

	char foo = this->appearsOnBackFence;
	this->pos_delta2.x = 0.0;
	this->pos_delta2.y = 16.0;
	this->pos_delta2.z = 0.0;

	this->pos.z = (foo == 0) ? 1500.0 : -2500.0;

	this->_518 = 2;

	//NOT NEEDED
	//this->doStateChange(&StateID_Walk);
}

int daMegaGoomba_c::onCreate() {
	/*80033230 daEnLkuribo_c::onCreate()*/
	this->setupBodyModel();
	this->max_speed.y = -4.0;
	this->direction = dSprite_c__getXDirectionOfFurthestPlayerRelativeToVEC3(this, this->pos);
	this->rot.y = (this->direction) ? 0xE000 : 0x2000;
	this->_518 = 2;

	isBigBoss = this->settings & 0xF;
	this->animationChr.setCurrentFrame(69.0);

	aPhysics.addToList();
	stalkAPhysics.addToList();
	leftTrapAPhysics.addToList();
	rightTrapAPhysics.addToList();

	this->_120 |= 0x200;

	this->_36D = 0;
	this->setupCollision();

	//HOMEMADE//
	speed.y = 0.0;
	dying = 0.0;
	rot.x = rot.z = 0;
	life = 3;
	already_hit = false;
	this->x_speed_inc = 0.1;
	this->pos.y -= 16.0;

	// 2.0 is good final speed
	this->XSpeed = 0.2;
	this->JumpHeight = 48.0;
	this->JumpDist = 64.0;
	this->JumpTime = 50.0;

	// doStateChange(&StateID_Grow);

	scale.x = 4.0f;
	scale.y = 4.0f;
	scale.z = 4.0f;
	setNewActivePhysicsRect(this, &this->scale);
	doStateChange(&StateID_Walk);

	this->onExecute();
	return true;
}

int daMegaGoomba_c::onDelete() {
	unstunPlayers();
	return true;
}

int daMegaGoomba_c::onExecute() {
	//80033450
	acState.execute();
	if (!hackOfTheCentury) {
		hackOfTheCentury = true;
	} else {
		checkZoneBoundaries(0);
	}
	updateModelMatrices();

	return true;
}

int daMegaGoomba_c::onDraw() {
	bodyModel.scheduleForDrawing();
	return true;
}


void daMegaGoomba_c::updateModelMatrices() {
	// This won't work with wrap because I'm lazy.
	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	bodyModel.setDrawMatrix(matrix);
	bodyModel.setScale(&scale);
	bodyModel.calcWorld(false);
}



// Shrink State
void daMegaGoomba_c::beginState_Shrink() {
	this->timer = 1.0;
	Xkey_count = 4;
	keysX[0] = (HermiteKey){  0.0, this->scale.y,        0.5 };
	keysX[1] = (HermiteKey){ 10.0, this->scale.y - 0.75, 0.5 };
	keysX[2] = (HermiteKey){ 20.0, this->scale.y - 0.35, 0.5 };
	keysX[3] = (HermiteKey){ 39.0, this->scale.y - 0.75, 0.5 };

	// disable being hit
	Vec tempVec = (Vec){0.0, 0.0, 0.0};
	setNewActivePhysicsRect(this,  &tempVec );
}
void daMegaGoomba_c::executeState_Shrink() { 
	this->timer += 1.0;
	
	float modifier = GetHermiteCurveValue(this->timer, this->keysX, Xkey_count);
	this->scale = (Vec){modifier, modifier, modifier};

	if(this->timer == 2.0)
		PlaySound(this, SE_EMY_KURIBO_L_DAMAGE_02);

	if (this->timer > 40.0) { doStateChange(&StateID_Walk); }
}
void daMegaGoomba_c::endState_Shrink() {
	// enable being hit
	setNewActivePhysicsRect(this, &this->scale);
	this->already_hit = false;
}



// Turn State
void daMegaGoomba_c::beginState_Turn() {
	this->direction ^= 1;
	this->speed.x = 0.0;
}
void daMegaGoomba_c::executeState_Turn() { 
	this->bodyModel._vf1C();

	this->HandleYSpeed();
	this->doSpriteMovement();

	/*this->_vf2D0();	//nullsub();*/
	int ret = SomeStrangeModification(this);

	if(ret & 1)
		this->speed.y = 0.0;
	if(ret & 4)
		this->pos.x = this->last_pos.x;
	DoStuffAndMarkDead(this, this->pos, 1.0);
	u16 amt = (this->direction == 0) ? 0x2000 : 0xE000;
	int done = SmoothRotation(&this->rot.y, amt, 0x80);
	if(done) {
		this->doStateChange(&StateID_Walk);
	}

	int frame = (int)(this->animationChr.getCurrentFrame() * 5.0);
	if ((frame == 100) || (frame == 325) || (frame == 550) || (frame == 775)) {
		ShakeScreen(StageScreen, 0, 1, 0, 0);
		stunPlayers();
		PlaySound(this, SE_BOSS_MORTON_GROUND_SHAKE);
	}

	if (isBigBoss) {
		if ((frame == 250) || (frame == 500) || (frame == 700) || (frame == 900))
			unstunPlayers();
	}
	else {
		if ((frame == 200) || (frame == 425) || (frame == 650) || (frame == 875))
			unstunPlayers();
	}
}
void daMegaGoomba_c::endState_Turn() {
	this->max_speed.x = (this->direction) ? -this->XSpeed : this->XSpeed;
}


// Walk State
void daMegaGoomba_c::beginState_Walk() {
	//inline this piece of code
	//YOU SUCK, WHOEVER ADDED THIS LINE OF CODE AND MADE ME SPEND AGES
	//HUNTING DOWN WHAT WAS BREAKING TURNING. -Treeki
	//this->direction = dSprite_c__getXDirectionOfFurthestPlayerRelativeToVEC3(this, this->pos);
	this->speed.x = this->speed.z = 0.0;
	this->max_speed.x = (this->direction) ? -this->XSpeed : this->XSpeed;
	this->speed.y = -4.0;
	this->y_speed_inc = -0.1875;
}
void daMegaGoomba_c::executeState_Walk() { 
	/* 800345e0 - daEnLkuribo_c::executeState_Walk() */
	this->bodyModel._vf1C();
	//HOMEMADE//
	this->HandleXSpeed();
	this->HandleYSpeed();
	this->doSpriteMovement();
	u16 amt = (this->direction == 0) ? 0x2000 : 0xE000;
	SmoothRotation(&this->rot.y, amt, 0x200);
	/*this->_vf2D0();	//nullsub();*/
	int ret = SomeStrangeModification(this);
	if(ret & 1)
		this->speed.y = 0.0;
	u32 bitfield = this->collMgr.outputMaybe;
	if(bitfield & (0x15<<this->direction)) {
		this->pos.x = this->last_pos.x;
		this->doStateChange(&StateID_Turn);
		//this->acState.setField10ToOne();
	}
	/*u32 bitfield2 = this->collMgr.adjacentTileProps[this->direction];
	if(bitfield2) {
		this->doStateChange(&StateID_Turn);
	}*/
	DoStuffAndMarkDead(this, this->pos, 1.0);


	int frame = (int)(this->animationChr.getCurrentFrame() * 5.0);
	if ((frame == 100) || (frame == 325) || (frame == 550) || (frame == 775)) {
		ShakeScreen(StageScreen, 0, 1, 0, 0);
		stunPlayers();
		PlaySound(this, SE_BOSS_MORTON_GROUND_SHAKE);
	}

	if (isBigBoss) {
		if ((frame == 250) || (frame == 500) || (frame == 700) || (frame == 900))
			unstunPlayers();
	}
	else {
		if ((frame == 200) || (frame == 425) || (frame == 650) || (frame == 875))
			unstunPlayers();
	}

	if(this->animationChr.isAnimationDone()) {
		this->animationChr.setCurrentFrame(0.0);

		int new_dir = dSprite_c__getXDirectionOfFurthestPlayerRelativeToVEC3(this, pos);
		if(this->direction != new_dir)
			doStateChange(&StateID_Turn);
	}
}
void daMegaGoomba_c::endState_Walk() { }





extern "C" void stunPlayer(void *, int);
extern "C" void unstunPlayer(void *);

void daMegaGoomba_c::stunPlayers() {
	for (int i = 0; i < 4; i++) {
		playerStunned[i] = false;

		dStageActor_c *player = GetSpecificPlayerActor(i);
		if (player) {
			if (player->collMgr.isOnTopOfTile() && player->currentZoneID == currentZoneID) {
				stunPlayer(player, 1);
				playerStunned[i] = true;
			}
		}
	}
}

void daMegaGoomba_c::unstunPlayers() {
	for (int i = 0; i < 4; i++) {
		dStageActor_c *player = GetSpecificPlayerActor(i);
		if (player && playerStunned[i]) {
			unstunPlayer(player);
		}
	}
}



void daMegaGoomba_c::dieOther_Begin() {
	animationChr.bind(&bodyModel, resFile.GetResAnmChr("damage"), true);
	bodyModel.bindAnim(&animationChr, 2.0f);
	speed.x = speed.y = speed.z = 0.0f;
	removeMyActivePhysics();

	PlaySound(this, SE_EMY_KURIBO_L_SPLIT_HPDP);

	rot.y = 0;
	counter_500 = 60;
}

void daMegaGoomba_c::dieOther_End() {
	dEn_c::dieOther_End();
}

void daMegaGoomba_c::dieOther_Execute() {
	bodyModel._vf1C();
	if (counter_500 == 0) {
		SpawnEffect("Wm_ob_icebreaksmk", 0, &pos, &(S16Vec){0,0,0}, &(Vec){5.0f, 5.0f, 5.0f});
		Delete(1);
	}
}


//
// processed\../src/bossFuzzyBear.cpp
//

#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include <stage.h>
#include "boss.h"


class daFuzzyBear_c : public daBoss {
	int onCreate();
	int onDelete();
	int onExecute();
	int onDraw();

	mHeapAllocator_c allocator;
	m3d::mdl_c bodyModel;
	nw4r::g3d::ResFile resFile;
	m3d::anmChr_c animationChr;

	int timer;
	char BigBossFuzzyBear;
	float Baseline;
	float AreaWidthLeft;
	float AreaWidthRight;
	float LaunchSpeedShort;
	float LaunchSpeedHigh;
	char dying;
	float storeSpeed;
	Vec initialPos;
	char RolyBounces;
	Vec RolyPos;
	char falldown;
	char damage;
	char roly;
	char isInvulnerable;
	
	static daFuzzyBear_c *build();

	void bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate);
	void setupBodyModel();
	void updateModelMatrices();

	void playerCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	void yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther);

	bool collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther);

	USING_STATES(daFuzzyBear_c);
	DECLARE_STATE(Grow);
	DECLARE_STATE(Bounce);
	// DECLARE_STATE(Needles);
	//DECLARE_STATE(Spray);
	DECLARE_STATE(RolyPoly);
	DECLARE_STATE(Wait);
	DECLARE_STATE(Outro);
};

daFuzzyBear_c *daFuzzyBear_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daFuzzyBear_c));
	return new(buffer) daFuzzyBear_c;
}



CREATE_STATE(daFuzzyBear_c, Grow);
CREATE_STATE(daFuzzyBear_c, Bounce);
// CREATE_STATE(daFuzzyBear_c, Needles);
//CREATE_STATE(daFuzzyBear_c, Spray);
CREATE_STATE(daFuzzyBear_c, RolyPoly);
CREATE_STATE(daFuzzyBear_c, Wait);
CREATE_STATE(daFuzzyBear_c, Outro);



void daFuzzyBear_c::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther) { DamagePlayer(this, apThis, apOther); }
void daFuzzyBear_c::yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther) { DamagePlayer(this, apThis, apOther); }
bool daFuzzyBear_c::collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther) { 
	return true; // added by skawo request

	if (this->isInvulnerable == 1) { return true; }

	this->timer = 0;
	PlaySound(this, SE_BOSS_KOOPA_FIRE_DISAPP);
	
	SpawnEffect("Wm_mr_fireball_hit", 0, &apOther->owner->pos, &(S16Vec){0,0,0}, &(Vec){1.0, 1.0, 1.0});
	this->damage++;
	if (this->damage > 14) { doStateChange(&StateID_Outro); }
	return true;
}
bool daFuzzyBear_c::collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther) { 
	apOther->someFlagByte |= 2;

	dActor_c *block = apOther->owner;
	dEn_c *mario = (dEn_c*)block;

	mario->speed.y = -mario->speed.y;
	mario->pos.y += mario->speed.y;

	if (mario->direction == 0) { mario->speed.x = 4.0; }
	else					  { mario->speed.x = -4.0; }
	
	mario->doSpriteMovement();
	mario->doSpriteMovement();
	return true;
}
bool daFuzzyBear_c::collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther) { 
	this->counter_504[apOther->owner->which_player] = 0;
	return this->collisionCat9_RollingObject(apThis, apOther);
}
bool daFuzzyBear_c::collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther) { 

	dActor_c *block = apOther->owner;
	dEn_c *blah = (dEn_c*)block;

	//OSReport("Fuzzy was hit by an actor %d (%p); its field 12C is %x\n", block->name, block, blah->_12C);
	if (block->name == AC_LIGHT_BLOCK) {
		//OSReport("It's a light block, and its state is %s\n", blah->acState.getCurrentState()->getName());
		if (blah->_12C & 3 || strcmp(blah->acState.getCurrentState()->getName(), "daLightBlock_c::StateID_Throw")) {
			//OSReport("Ignoring this!\n");
			return true;
		}
	}

	if (blah->direction == 0) { blah->direction = 1; this->roly = 1; }
	else					  { blah->direction = 0; this->roly = 0; }

	blah->speed.x = -blah->speed.x;
	blah->pos.x += blah->speed.x;

	if (blah->speed.y < 0) {
		blah->speed.y = -blah->speed.y; }
	
	blah->doSpriteMovement();
	blah->doSpriteMovement();
	
	if (this->isInvulnerable == 1) { 
		
		if (blah->direction == 0) { blah->direction = 1; }
		else					  { blah->direction = 0; }
		
		return true; 
	}	
	
	this->pos.x += blah->speed.x;
		
	this->timer = 0; 
	this->damage = this->damage + 5;
	
	
	PlaySound(this, SE_EMY_BLOW_PAKKUN_DOWN);
	SpawnEffect("Wm_mr_kickhit", 0, &blah->pos, &(S16Vec){0,0,0}, &(Vec){1.0, 1.0, 1.0});
	
	if (this->damage > 14) { doStateChange(&StateID_Outro); } 
	else { doStateChange(&StateID_RolyPoly); }
	return true;
}
bool daFuzzyBear_c::collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther) { 

	if (this->isInvulnerable == 1) { return true; }

	dActor_c *block = apOther->owner;
	dEn_c *blah = (dEn_c*)block;

	if (blah->direction == 0) { blah->direction = 1; this->roly = 1; }
	else					  { blah->direction = 0; this->roly = 0; }

	PlaySound(this, SE_EMY_BIG_PAKKUN_DAMAGE_1);
	this->timer = 0; 
	this->damage += 5;
	
	SpawnEffect("Wm_mr_kick_glow", 0, &apOther->owner->pos, &(S16Vec){0,0,0}, &(Vec){1.0, 1.0, 1.0});
	
	if (this->damage > 14) { doStateChange(&StateID_Outro); } 
	else { doStateChange(&StateID_RolyPoly); }
	return true;
}

bool daFuzzyBear_c::collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther) {
	return false;
}
bool daFuzzyBear_c::collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther) {
	return true;
}
bool daFuzzyBear_c::collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther) {
	DamagePlayer(this, apThis, apOther);
	return true;
}




void daFuzzyBear_c::bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate) {
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr(name);
	this->animationChr.bind(&this->bodyModel, anmChr, unk);
	this->bodyModel.bindAnim(&this->animationChr, unk2);
	this->animationChr.setUpdateRate(rate);
}


void daFuzzyBear_c::setupBodyModel() {
	allocator.link(-1, GameHeaps[0], 0, 0x20);

	this->resFile.data = getResource("chorobon", "g3d/chorobon.brres");
	nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("chorobon");
	bodyModel.setup(mdl, &allocator, 0x224, 1, 0);
	SetupTextures_Enemy(&bodyModel, 0);

	bool ret;
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr("run");
	ret = this->animationChr.setup(mdl, anmChr, &this->allocator, 0);

	allocator.unlink();
}


int daFuzzyBear_c::onCreate() {

	setupBodyModel();


	this->BigBossFuzzyBear = this->settings >> 28;
	this->scale = (Vec){1.0, 1.0, 1.0};


	ActivePhysics::Info HitMeBaby;
	HitMeBaby.xDistToCenter = 0.0;
	HitMeBaby.yDistToCenter = 0.0;

	if (BigBossFuzzyBear == 0) {
		HitMeBaby.xDistToEdge = 30.0;
		HitMeBaby.yDistToEdge = 30.0; }
	else {
		HitMeBaby.xDistToEdge = 35.0;
		HitMeBaby.yDistToEdge = 35.0; }

	HitMeBaby.category1 = 0x3;
	HitMeBaby.category2 = 0x0;
	HitMeBaby.bitfield1 = 0x4F;
	HitMeBaby.bitfield2 = 0x8028E;
	HitMeBaby.unkShort1C = 0;
	HitMeBaby.callback = &dEn_c::collisionCallback;


	this->aPhysics.initWithStruct(this, &HitMeBaby);
	this->aPhysics.addToList();

	
	this->pos.y = this->pos.y + 6;
	this->rot.x = 0; // X is vertical axis
	this->rot.y = 0; // Y is horizontal axis
	this->rot.z = 0; // Z is ... an axis >.>
	this->direction = 0; // Heading left.

	pos.z = 3000.0f;
	
	this->speed.x = 0;
	this->LaunchSpeedShort = ((this->settings >> 20) & 0xF) * 10.0;
	this->LaunchSpeedHigh = ((this->settings >> 24) & 0xF) * 10.0;
	
	this->AreaWidthRight = this->settings & 0xFF;
	this->AreaWidthLeft = (this->settings >> 8) & 0xFF;
	
	this->initialPos = this->pos;
	this->storeSpeed = 0;
	this->falldown = 0;
	this->roly = 0;
	this->damage = 0;
	this->isInvulnerable = 0;
	this->dying = 0;
	this->disableEatIn();


	bindAnimChr_and_setUpdateRate("run", 1, 0.0, 1.0);
		
	doStateChange(&StateID_Grow);

	this->onExecute();
	return true;
}

int daFuzzyBear_c::onDelete() {
	return true;
}

int daFuzzyBear_c::onExecute() {
	acState.execute();
	updateModelMatrices();

	if(this->animationChr.isAnimationDone())
		this->animationChr.setCurrentFrame(0.0);
	
	return true;
}


int daFuzzyBear_c::onDraw() {
	bodyModel.scheduleForDrawing();
	bodyModel._vf1C();
	return true;
}


void daFuzzyBear_c::updateModelMatrices() {
	// This won't work with wrap because I'm lazy.
	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	bodyModel.setDrawMatrix(matrix);
	bodyModel.setScale(&scale);
	bodyModel.calcWorld(false);
}


// Grow State

void daFuzzyBear_c::beginState_Grow() { 
	this->timer = 0;

	SetupKameck(this, Kameck);
	this->scale = (Vec){1.0, 1.0, 1.0};
}

void daFuzzyBear_c::executeState_Grow() { 

	this->timer += 1;
	
	bool ret;
	if (BigBossFuzzyBear == 1) {
		ret = GrowBoss(this, Kameck, 1.0, 3.0, 25, this->timer); }
	else {
		ret = GrowBoss(this, Kameck, 1.0, 2.5, 18, this->timer); }

	if (ret) { doStateChange(&StateID_Bounce);	}

}
void daFuzzyBear_c::endState_Grow() { 
	this->Baseline = this->pos.y;

	CleanupKameck(this, Kameck);
}



// Bounce State

void daFuzzyBear_c::beginState_Bounce() { 

	if (this->direction == 0) { this->speed.x = 1.0; }
	else 					 { this->speed.x = -1.0; }
	
	if (this->storeSpeed != 0) { this->speed.x = this->storeSpeed; }
	
	this->timer = 20;
}
void daFuzzyBear_c::executeState_Bounce() { 

	float wallDistance, scaleDown, scaleUp, scaleBase;

	if (BigBossFuzzyBear == 0) {
		wallDistance = 32.0;
		scaleDown = 24.0;
		scaleUp = 10.0;
		scaleBase = 2.5;
	}
	else {
		wallDistance = 38.0;
		scaleDown = 32.0;
		scaleUp = 12.0;
		scaleBase = 3.0;
	}


	if (this->falldown == 1) { this->speed.x = 0; this->timer = 0; }	

	
	// Check for walls
	
	if (this->pos.x <= this->initialPos.x - ((17 * 16.0) + wallDistance))  { // Hit left wall, head right.
		this->speed.x = -this->speed.x;
		this->direction = 1;
		this->pos.x = this->pos.x + 1.0; }
		
	if (this->pos.x >= this->initialPos.x + ((7.5 * 16.0) - wallDistance))  { // Hit right wall, head left.
		this->speed.x = -this->speed.x;
		this->direction = 0;
		this->pos.x = this->pos.x - 1.0; }
	
	
	// Check if we're on the ground.
	
	if (this->pos.y < this->Baseline) {

		this->falldown = 0;
		
		this->timer = this->timer + 1;
		if (this->speed.x != 0) {
			this->storeSpeed = this->speed.x; }
		this->speed.x = 0;
		this->speed.y = 0;
		
		
		if (this->timer < 10) {
			float modifier;
			modifier = scaleBase - (this->timer * 0.1);
			
			this->scale.y = modifier;
			this->pos.y = this->pos.y + (scaleDown/10.0);
			if (this->pos.y > this->Baseline) { this->pos.y = this->Baseline - 1.0; }
		}
		else if (this->timer == 10) { 
			Vec tempPos = (Vec){this->pos.x, this->pos.y - wallDistance, 5500.0};
			SpawnEffect("Wm_mr_beachlandsmk", 0, &tempPos, &(S16Vec){0,0,0}, &(Vec){1.0, 1.0, 1.0});
		}
		else {
			float modifier;
			modifier = (scaleBase - 1.0) + ((this->timer - 10) * 0.1);
			
			this->scale.y = modifier;
			this->pos.y = this->pos.y + (scaleUp/10.0);
			if (this->pos.y > this->Baseline) { this->pos.y = this->Baseline - 1.0; }
			PlaySound(this, SE_PLY_JUMPDAI);
		}
			
		if (this->timer > 20) { 
			
			int randChoice;
			
			randChoice = GenerateRandomNumber(5);
			if (randChoice == 0) { doStateChange(&StateID_Wait); }

			randChoice = GenerateRandomNumber(4);
			if (randChoice == 0) { this->speed.y = 8.0; }
			else { this->speed.y = 6.0; }
		
			this->timer = 0;
			this->pos.y = this->Baseline + 1;
			
			this->speed.x = this->storeSpeed;
		 }
	}
	
	else { this->speed.y = this->speed.y - 0.1; } // Gravity 

		
	this->HandleXSpeed();
	this->HandleYSpeed();

	this->UpdateObjectPosBasedOnSpeedValuesReal();

}

void daFuzzyBear_c::endState_Bounce() { }





// Needles State - shoots out some black icicles

// void daFuzzyBear_c::beginState_Needles() {
// 	this->timer = 0;
// 	this->speed.y = 0;	
// 	this->speed.x = 0;
// 	OSReport("Fuzzy Needle State Begin"); 
// }
// void daFuzzyBear_c::executeState_Needles() { 
// 	float origScale;

// 	this->speed.y = 0;	
// 	this->speed.x = 0;

// 	if (BigBossFuzzyBear == 0) {
// 		origScale = 2.5;
// 	}
// 	else {
// 		origScale = 3.0;
// 	}

// 	this->timer = this->timer + 1;
// 	OSReport("Needle Timer: %d", this->timer); 

// 	if (this->timer <= 120) {
// 		this->scale.y = (sin(this->timer * 3.14 / 5.0) / 2.0) + origScale; // 3 shakes per second, exactly 24 shakes overall
// 		this->scale.x = (sin(this->timer * 3.14 / 5.0) / 2.0) + origScale; // 3 shakes per second, exactly 24 shakes overall
	
// 		if (this->timer == 30) {
// 			dStageActor_c *spawner = CreateActor(339, 0, this->pos, 0, 0);
// 			spawner->speed.x = -6.0;
// 			spawner->speed.y = 0.0;
// 			spawner->scale = (Vec){1.0, 1.0, 1.0};
// 		}

// 		if (this->timer == 45) {
// 			dStageActor_c *spawner = CreateActor(339, 0, this->pos, 0, 0);
// 			spawner->speed.x = 6.0;
// 			spawner->speed.y = 6.0;
// 			spawner->scale = (Vec){1.0, 1.0, 1.0};
// 		}

// 		if (this->timer == 60) {
// 			dStageActor_c *spawner = CreateActor(339, 0, this->pos, 0, 0);
// 			spawner->speed.x = 0.0;
// 			spawner->speed.y = 6.0;
// 			spawner->scale = (Vec){1.0, 1.0, 1.0};
// 		}

// 		if (this->timer == 75) {
// 			dStageActor_c *spawner = CreateActor(339, 0, this->pos, 0, 0);
// 			spawner->speed.x = -6.0;
// 			spawner->speed.y = 6.0;
// 			spawner->scale = (Vec){1.0, 1.0, 1.0};
// 		}

// 		if (this->timer == 90) {
// 			dStageActor_c *spawner = CreateActor(339, 0, this->pos, 0, 0);
// 			spawner->speed.x = -6.0;
// 			spawner->speed.y = 0.0;
// 			spawner->scale = (Vec){1.0, 1.0, 1.0};
// 		}
// 	}
// 	else { doStateChange(&StateID_Bounce); }

// 	this->HandleXSpeed();
// 	this->HandleYSpeed();

// 	this->UpdateObjectPosBasedOnSpeedValuesReal();

// }
// void daFuzzyBear_c::endState_Needles() { OSReport("Fuzzy Needle State End"); }

// Spray State - jumps in the air and shakes out some small fuzzies

/*void daFuzzyBear_c::beginState_Spray() { 
	this->timer = 0; 
	this->speed.y = 7.0;
	this->speed.x = 0.0;
}
void daFuzzyBear_c::executeState_Spray() { 
		
	this->speed.x = 0.0;

	if (this->speed.y < 1.0) {
		this->speed.y = 0;
		
		if (this->timer < 120) {
			
			this->rot.y = sin(this->timer * 3.14 / 5) * 4000; // 3 shakes per second, exactly 24 shakes overall

			int randChoice;
			randChoice = GenerateRandomNumber(18); // 1.3 Fuzzies per second, 6 fuzzies overall
		
			if (randChoice == 0) { 
				int randChoiceX, randChoiceY;
				randChoiceX = GenerateRandomNumber(92);
				randChoiceY = GenerateRandomNumber(48);
				
				float xa, ya;
				xa = randChoiceX - 48.0;
				ya = randChoiceY - 24.0;
				
				CreateActor(144, 0, (Vec){this->pos.x + xa, this->pos.y + ya, this->pos.z}, 0, 0);
			} 
		}

		else { doStateChange(&StateID_Bounce); }

		this->timer = this->timer + 1;
		
	}
	
	else { this->speed.y = this->speed.y - 0.1; } // Gravity 


	this->HandleXSpeed();
	this->HandleYSpeed();

	this->UpdateObjectPosBasedOnSpeedValuesReal();

}
void daFuzzyBear_c::endState_Spray() { 
	this->rot.y = 0;
	this->timer = 20;
	this->falldown = 1;	
}*/


// Roly Poly State - Rolls from left to right, bounces off both walls, and returns to original position.

void daFuzzyBear_c::beginState_RolyPoly() { 
	
	this->isInvulnerable = 1;
	
	if (this->roly == 1) { 
		this->direction = 1;
		this->speed.x = 12.0; }
	else 				 { 
		this->direction = 0;
		this->speed.x = -12.0; }
	
	this->speed.y = 0;
	this->RolyBounces = 0;
	this->RolyPos = this->pos;

	if (BigBossFuzzyBear == 0) {
		this->scale = (Vec){2.5, 2.5, 2.5};
	}
	else {
		this->scale = (Vec){3.0, 3.0, 3.0};
	}

	this->timer = 0;
}
void daFuzzyBear_c::executeState_RolyPoly() { 
	float wallDistance, scaleDown, scaleUp;

	PlaySound(this, SE_OBJ_TEKKYU_G_CRASH);
	
	if (this->pos.y > this->Baseline) { this->pos.y -= 2.0; }
	else { 
		this->pos.y = this->Baseline - 1.0;
		
		Vec tempPos = (Vec){this->pos.x, this->pos.y - 34.0, 5500.0};
		
		if (this->timer & 0x1) { 
			SpawnEffect("Wm_ob_icehitsmk", 0, &tempPos, &(S16Vec){0,0,0}, &(Vec){0.4, 0.4, 0.4}); 
		}
	 }
	
	if (this->direction == 0) { // is even
		this->pos.x = this->pos.x - 3.0; }
	else { // is odd
		this->pos.x = this->pos.x + 3.0; }

	if (BigBossFuzzyBear == 0) {
		wallDistance = 38.0;
	}
	else {
		wallDistance = 50.0;
	}
	
	this->timer += 1;

	if (this->pos.x <= this->initialPos.x - ((17 * 16.0) + wallDistance))  { // Hit left wall, head right.
		this->speed.x = -this->speed.x;
		this->direction = 1;
		this->pos.x = this->pos.x + 1.0; 
		this->RolyBounces = this->RolyBounces + 1;
		}
		
	if (this->pos.x >= this->initialPos.x + ((7.5 * 16.0) - wallDistance))  { // Hit right wall, head left.
		this->speed.x = -this->speed.x;
		this->direction = 0;
		this->pos.x = this->pos.x - 1.0; 
		this->RolyBounces = this->RolyBounces + 1;
		}

	
	if (this->direction == 1) { this->rot.z = this->rot.z - 0x400; } 
	else 					  { this->rot.z = this->rot.z + 0x400; } 
		
	if (this->RolyBounces == 2) {
		if ((this->pos.x > this->RolyPos.x -20.0) && (this->pos.x < this->RolyPos.x + 20.0)) {
			this->speed.x = 0;
			if (this->rot.z == 0) { doStateChange(&StateID_Bounce); } } 
	}



//	this->HandleXSpeed();
//	this->HandleYSpeed();

//	this->doSpriteMovement();
//	this->UpdateObjectPosBasedOnSpeedValuesReal();
	
 }
void daFuzzyBear_c::endState_RolyPoly() { 
	this->rot.z = 0; 
	this->isInvulnerable = 0;
	this->timer = 0;
}





void daFuzzyBear_c::beginState_Wait() { this->timer = 0;}
void daFuzzyBear_c::executeState_Wait() { 
	
	this->timer = this->timer + 1;

	if (this->timer > 60) {	
		doStateChange(&StateID_Bounce);
		//int randChoice;
		
		//if (BigBossFuzzyBear == 1) { doStateChange(&StateID_Spray); }
		//else 					   { doStateChange(&StateID_Bounce); }
	}	
} 
void daFuzzyBear_c::endState_Wait() { }



void daFuzzyBear_c::beginState_Outro() { 
	OutroSetup(this);
}
void daFuzzyBear_c::executeState_Outro() {

	if (this->dying == 1) { 
		if (this->timer > 180) { 
			ExitStage(WORLD_MAP, 0, BEAT_LEVEL, MARIO_WIPE); 
		}
		if (this->timer == 60) { PlayerVictoryCries(this); }	
		
		this->timer += 1;
		return; 
	}	

	bool ret;
	ret = ShrinkBoss(this, &this->pos, 2.75, this->timer);

	if (ret == true) 	{ 
		BossExplode(this, &this->pos); 
		this->dying = 1;
		this->timer = 0;	
	}

	this->timer += 1;

}
void daFuzzyBear_c::endState_Outro() { }






//
// processed\../src/bossThwompaDomp.cpp
//

#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include <stage.h>
#include "boss.h"

// Externs
	extern "C" int posIsInZone(Vec,float*,float*,u8 zone);
	extern "C" void* ScreenPositionClass;
	extern "C" int SpawnThwompEffects(dEn_c *);

	extern "C" void* SoundRelatedClass;
	extern "C" Vec ConvertStagePositionIntoScreenPosition__Maybe(Vec);
	extern "C" void AnotherSoundRelatedFunction(void*,SFX,Vec,int);


class daEnMegaDosun_c : public daBoss {
	int onCreate();
	int onDelete();
	int onExecute();
	int onDraw();

	static daEnMegaDosun_c *build();

	mHeapAllocator_c allocator;		// _524

	nw4r::g3d::ResFile resFile;		// _540
	m3d::mdl_c bodyModel;			// _544
	m3d::anmVis_c anmVis;			// _584

	lineSensor_s belowSensor;
	float shakePosXoffset;			// _5CC shakePosXoffset
	u16 puruMoveCounter;			// _5D8
	u16 shakeIndex;					// _5DA 0=shake,1=normal
	u32 countdownTimer;				// _5DC

	int timer;
	char notFalling;
	char dying;
	float leftBuffer;
	float rightBuffer;
	float topBuffer;
	bool isOutofScreen;
	Vec OutOfScreenPosition;

	void setupBodyModel();

	void playerCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther);

	void powBlockActivated(bool isNotMPGP);

	USING_STATES(daEnMegaDosun_c);
	DECLARE_STATE(UpWait);
	DECLARE_STATE(DownMoveWait);
	DECLARE_STATE(PuruMove);
	DECLARE_STATE(DownMove);
	DECLARE_STATE(DownWait);
	DECLARE_STATE(UpMove);
	DECLARE_STATE(Grow);
	DECLARE_STATE(Outro);
};

// States
	CREATE_STATE(daEnMegaDosun_c, UpWait);
	CREATE_STATE(daEnMegaDosun_c, DownMoveWait);
	CREATE_STATE(daEnMegaDosun_c, PuruMove);
	CREATE_STATE(daEnMegaDosun_c, DownMove);
	CREATE_STATE(daEnMegaDosun_c, DownWait);
	CREATE_STATE(daEnMegaDosun_c, UpMove);
	CREATE_STATE(daEnMegaDosun_c, Grow);
	CREATE_STATE(daEnMegaDosun_c, Outro);

daEnMegaDosun_c *daEnMegaDosun_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daEnMegaDosun_c));
	return new(buffer) daEnMegaDosun_c;
}

void daEnMegaDosun_c::powBlockActivated(bool isNotMPGP) { }

// Collisions

	void daEnMegaDosun_c::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
		DamagePlayer(this, apThis, apOther);
	}
	bool daEnMegaDosun_c::collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther) {
		return true;
	}
	bool daEnMegaDosun_c::collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther) {
		return false;
	}
	bool daEnMegaDosun_c::collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther) {
		DamagePlayer(this, apThis, apOther);
		return true;
	}
	bool daEnMegaDosun_c::collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther) {
		return true;
	}
	bool daEnMegaDosun_c::collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther) {
		return true;
	}
	bool daEnMegaDosun_c::collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther) {
		return true;
	}
	bool daEnMegaDosun_c::collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther) {
		DamagePlayer(this, apThis, apOther);
		return true;
	}


void daEnMegaDosun_c::setupBodyModel() {
	allocator.link(-1, GameHeaps[0], 0, 0x20);

	this->resFile.data = getResource("dossun", "g3d/t00.brres");
	nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("dossun_big");
	bodyModel.setup(mdl, &allocator, 0x60, 1, 0);
	SetupTextures_Enemy(&bodyModel, 0);

	nw4r::g3d::ResAnmVis anmRes = this->resFile.GetResAnmVis("dossun_big");
	this->anmVis.setup(mdl, anmRes, &this->allocator, 0);
	this->anmVis.bind(&bodyModel, anmRes, 1);

	allocator.unlink();
}

int daEnMegaDosun_c::onCreate() {

	this->setupBodyModel();

	this->_36D = 0;			// byte

	this->frzMgr.setSomething(1,1,1);	//@0x809f5c5c

	this->pos.y -= 21.0;
	this->pos.z = -280.0;		// behind layer1 (hides spikes)

	belowSensor.flags =
		SENSOR_10000000 | SENSOR_1000000 | SENSOR_BREAK_BRICK |
		SENSOR_BREAK_BLOCK | SENSOR_100 | SENSOR_LINE;
	belowSensor.lineA = -31 << 12;
	belowSensor.lineB = 31 << 12;
	belowSensor.distanceFromCenter = 5 << 12;

	ActivePhysics::Info hm;
	hm.xDistToCenter = 0.0;
	hm.yDistToCenter = 41.0;
	hm.xDistToEdge = 36.0;
	hm.yDistToEdge = 38.0;
	hm.category1 = 0x03;
	hm.category2 = 0x00;
	hm.bitfield1 = 0x0000004F;
	hm.bitfield2 = 0x0008820E;
	hm.unkShort1C = 0x0100;
	hm.callback = &dEn_c::collisionCallback;
	this->aPhysics.initWithStruct(this, &hm);

	lineSensor_s aboveSensor(-31 << 12, 31 << 12, -31 << 12);
	this->collMgr.init(this, &belowSensor, &aboveSensor, 0);

	this->pos_delta2.x = 0.0;
	this->pos_delta2.y = 36.0;
	this->pos_delta2.z = 0.0;
	this->_320 = 0.0;
	this->_324 = 48.0;

	this->aPhysics.addToList();

	this->scale.x = 1.0;
	this->scale.y = 1.0;
	this->scale.z = 1.0;
	this->max_speed.x = 0.0;
	this->max_speed.y = -8.0;
	this->max_speed.z = 0.0;
	this->y_speed_inc = -0.25;
	this->rot.x = 0;
	this->rot.y = 0;
	this->rot.z = 0;
	this->notFalling = 0;
	this->direction = 0;
	this->countdownTimer = 0;
	this->isOutofScreen = false;

	// Measured in half tiles
	this->leftBuffer	= this->pos.x - (((float)((this->settings >> 24) & 0xFF) - 5.0) * 8.0);  //nyb 5-6 LEFT
	this->rightBuffer	= this->pos.x + (((float)((this->settings >> 16) & 0xFF) - 3.0) * 8.0);  //nyb 7-8 RIGHT
	this->topBuffer		= this->pos.y + (((float)((this->settings >> 8) & 0xFF) - 8.0) * 8.0);   //nyb 9-10 TOP


	// Boss Thwomp settings
	//
	//	nybble 5-6 		- Left Buffer in half tiles (minimum is 5 due to thwomp width)
	//	nybble 7-8 		- Left Buffer in half tiles (minimum is 3 due to thwomp width)
	//	nybble 9-10		- Top Buffer in half tiles (minimum is 8 due to thwomp height)
	//


	this->doStateChange(&StateID_Grow);

	this->onExecute();
	return true;
}

int daEnMegaDosun_c::onExecute() {
	acState.execute();

	if (this->isOutofScreen == false) {
		float rect[] = {this->_320, this->_324, this->spriteSomeRectX, this->spriteSomeRectY};
		int ret = this->outOfZone(this->pos, (float*)&rect, this->currentZoneID);
		if(ret) {
			this->OutOfScreenPosition = this->pos;

			this->isOutofScreen = true;
			doStateChange(&StateID_Outro);
		}
	}

	return true;
}

int daEnMegaDosun_c::onDraw() {
	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	bodyModel.setDrawMatrix(matrix);
	bodyModel.setScale(&scale);
	bodyModel.calcWorld(false);

	bodyModel.scheduleForDrawing();
	return true;
}

int daEnMegaDosun_c::onDelete() {
	return true;
}


// Grow State

	void daEnMegaDosun_c::beginState_Grow() { 
		this->scale = (Vec){0.5, 0.5, 0.5};
		this->timer = 0;

		SetupKameck(this, Kameck);
	}

	void daEnMegaDosun_c::executeState_Grow() { 
			
		bool ret;
		ret = GrowBoss(this, Kameck, 0.5, 1.0, 0, this->timer);

		if (ret) { 	
			PlaySound(this, SE_EMY_BIG_DOSSUN);
			doStateChange(&StateID_UpMove); 
		}	
		this->timer += 1;		
	}
	void daEnMegaDosun_c::endState_Grow() { 
		CleanupKameck(this, Kameck);
	}

// StateID_UpWait
	void daEnMegaDosun_c::beginState_UpWait() {
		belowSensor.flags =
			SENSOR_10000000 | SENSOR_1000000 | SENSOR_BREAK_BRICK |
			SENSOR_BREAK_BLOCK | SENSOR_100 | SENSOR_LINE;
		this->timer = 0;
	}
	void daEnMegaDosun_c::executeState_UpWait() {
		if(this->countdownTimer != 0) {
			this->countdownTimer--;
			return;
		}

		if (this->pos.x > this->rightBuffer) {
			SpawnEffect("Wm_en_dossunfall02", 0, &(Vec){this->pos.x + 38.0, this->pos.y + 32.0, 5500.0}, &(S16Vec){0,0,0}, &(Vec){1.0, 1.0, 1.0});
			PlaySoundAsync(this, SE_OBJ_TEKKYU_G_CRASH);
			
			this->direction = 0;
		}

		if (this->pos.x < this->leftBuffer) {
			SpawnEffect("Wm_en_dossunfall02", 0, &(Vec){this->pos.x - 40.0, this->pos.y + 32.0, 5500.0}, &(S16Vec){0,0,0}, &(Vec){1.0, 1.0, 1.0});
			PlaySoundAsync(this, SE_OBJ_TEKKYU_G_CRASH);

			this->direction = 1;
		}

		this->pos.x += (direction) ? 1.2 : -1.2;

		if (this->notFalling == 0) {
			if(this->CheckIfPlayerBelow(40.0, 256.0)) {
				this->doStateChange(&StateID_DownMoveWait);
				this->speed.y = 0.0;
			}
		}
		
		if (this->timer == 30) {
			this->notFalling = 0;
		}

		this->timer += 1;
	}
	void daEnMegaDosun_c::endState_UpWait() {
		this->notFalling = 1;
		return;
	}

// StateID_DownMoveWait
	void daEnMegaDosun_c::beginState_DownMoveWait() {
		this->anmVis.playState = 1;
		this->bodyModel.bindAnim(&this->anmVis, 0.5);
	}
	void daEnMegaDosun_c::executeState_DownMoveWait() {
		if(this->anmVis.isAnimationDone())
			this->doStateChange(&StateID_PuruMove);
		this->anmVis.process();
	}
	void daEnMegaDosun_c::endState_DownMoveWait() {
		return;
	}

// StateID_PuruMove
	void daEnMegaDosun_c::beginState_PuruMove() {
		this->puruMoveCounter = 8;
		this->shakeIndex = 0;
	}
	void daEnMegaDosun_c::executeState_PuruMove() {
		if(this->puruMoveCounter == 0) {
			this->shakePosXoffset = 0.0;
			this->doStateChange(&StateID_DownMove);
			return;
		}

		this->puruMoveCounter--;
		if((this->puruMoveCounter & 2) == 0)
			return;

		float _array[] = {2.0, 0.0};
		this->shakeIndex ^= 1;
		this->shakePosXoffset = _array[this->shakeIndex];
	}
	void daEnMegaDosun_c::endState_PuruMove() {
		return;
	}

// StateID_DownMove
	void daEnMegaDosun_c::beginState_DownMove() {
		this->speed.y = 0.0;
	}
	void daEnMegaDosun_c::executeState_DownMove() {
		this->HandleYSpeed();
		this->UpdateObjectPosBasedOnSpeedValuesReal();

		//FIXME what do I do? - bottom detection
		int ret = this->collMgr.calculateBelowCollisionWithSmokeEffect();
		if(!ret)
			return;

		if(!(ret & 0x400000)) {
			this->doStateChange(&StateID_DownWait);
			this->countdownTimer = 0x40;
			this->speed.y = 0.0;
			ShakeScreen(ScreenPositionClass, 0, 1, 0, 0);
			PlaySoundAsync(this, SE_EMY_BIG_DOSSUN);
		}
		else {
			belowSensor.flags = SENSOR_LINE;
			this->speed.y = 0.0;
			ShakeScreen(ScreenPositionClass, 0, 1, 0, 0);
			this->collMgr.clear2();
			PlaySoundAsync(this, SE_EMY_BIG_DOSSUN);
		}

		SpawnThwompEffects(this);
		Vec p = ConvertStagePositionIntoScreenPosition__Maybe(this->pos);
		AnotherSoundRelatedFunction(SoundRelatedClass, SE_EMY_BIG_DOSSUN, p, 0);
		//SoundRelatedClass.AnotherSoundRelatedFunction(SE_EMY_BIG_DOSSUN, p, 0);
	}
	void daEnMegaDosun_c::endState_DownMove() {
		return;
	}

// StateID_DownWait
	void daEnMegaDosun_c::beginState_DownWait() {
		return;
	}
	void daEnMegaDosun_c::executeState_DownWait() {
		if(this->countdownTimer == 0) {
			this->doStateChange(&StateID_UpMove);
		}
		else {
			this->countdownTimer--;
			if(this->countdownTimer == 0x20) {
				this->anmVis.playState = 3;
			}
		}

		this->anmVis.process();
	}
	void daEnMegaDosun_c::endState_DownWait() {
		return;
	}

// StateID_UpMove
	void daEnMegaDosun_c::beginState_UpMove() {
		this->collMgr.clear2();
	}
	void daEnMegaDosun_c::executeState_UpMove() {
		// this->speed.y = 0.0;
		// this->UpdateObjectPosBasedOnSpeedValuesReal();

		if (this->pos.y > this->topBuffer) {
			this->doStateChange(&StateID_UpWait);
			PlaySoundAsync(this, SE_OBJ_TEKKYU_L_CRASH);
			this->countdownTimer = 0xc;
		}
		else {
			this->pos.y += 1.5;
		}
	}
	void daEnMegaDosun_c::endState_UpMove() {
		return;
	}

// Outro
	void daEnMegaDosun_c::beginState_Outro() { 
		OutroSetup(this);
		this->timer = 0;

		this->speed.y = 0.0;
		this->y_speed_inc = 0.0;
	}
	void daEnMegaDosun_c::executeState_Outro() {

		this->pos.y = this->OutOfScreenPosition.y + 280.0;

		if (this->timer == 0) {

			nw4r::snd::SoundHandle handle;
			PlaySoundWithFunctionB4(SoundRelatedClass, &handle, SE_EMY_BIG_DOSSUN_DEAD, 1);

			SpawnEffect("Wm_mr_stockitemuse_b", 0, &this->pos, &(S16Vec){0,0,0}, &(Vec){2.0, 2.0, 2.0});
			SpawnEffect("Wm_mr_stockitemuse_c", 0, &this->pos, &(S16Vec){0,0,0}, &(Vec){2.0, 2.0, 2.0});
		}

		if (this->timer == 60) {
			nw4r::snd::SoundHandle handle;
			PlaySoundWithFunctionB4(SoundRelatedClass, &handle, STRM_BGM_SHIRO_BOSS_CLEAR, 1);
			BossGoalForAllPlayers();
		}
		
		if (this->timer == 120) {
			
			PlayerVictoryCries(this);
		}	
				
		if (this->timer > 240) {
			ExitStage(WORLD_MAP, 0, BEAT_LEVEL, MARIO_WIPE);
		}

		this->timer += 1;
	}
	void daEnMegaDosun_c::endState_Outro() { }

//
// processed\../src/bossRamboo.cpp
//

#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include <stage.h>
#include "boss.h"
#include "player.h"

class daRamboo_c : public daBoss {
	int onCreate();
	int onDelete();
	int onExecute();
	int onDraw();

	mHeapAllocator_c allocator;
	m3d::mdl_c bodyModel;
	m3d::mdl_c hideModel;
	m3d::mdl_c fogModel;

	nw4r::g3d::ResFile resFile;
	m3d::anmChr_c anmFog;
	m3d::anmChr_c anmA;
	m3d::anmChr_c anmB;

	nw4r::g3d::ResAnmTexSrt resTexSrt;
	m3d::anmTexSrt_c fogSrt;

	int timer;
	int ytimer;
	char Hiding;
	char dying;
	float Baseline;
	bool fleeFast;

	u64 eventFlag;

	static daRamboo_c *build();

	void bindAnimChr_and_setUpdateRates(const char* name, m3d::anmChr_c &animationChr, m3d::mdl_c &model, float rate);
	void setupModels();
	void updateModelMatrices();

	void playerCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther);


	USING_STATES(daRamboo_c);
	DECLARE_STATE(Grow);
	DECLARE_STATE(Advance);
	DECLARE_STATE(Wait);
	DECLARE_STATE(Flee);
	DECLARE_STATE(Outro);
};

daRamboo_c *daRamboo_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daRamboo_c));
	return new(buffer) daRamboo_c;
}


CREATE_STATE(daRamboo_c, Grow);
CREATE_STATE(daRamboo_c, Advance);
CREATE_STATE(daRamboo_c, Wait);
CREATE_STATE(daRamboo_c, Flee);
CREATE_STATE(daRamboo_c, Outro);

extern "C" void *EN_LandbarrelPlayerCollision(dEn_c* t, ActivePhysics *apThis, ActivePhysics *apOther);

void daRamboo_c::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther) { 
	EN_LandbarrelPlayerCollision(this, apThis, apOther);
	DamagePlayer(this, apThis, apOther); 

	fleeFast = false;
	doStateChange(&StateID_Flee);
}
bool daRamboo_c::collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther) { 
	SpawnEffect("Wm_en_obakedoor_sm", 0, &apOther->owner->pos, &(S16Vec){0,0,0}, &(Vec){0.5, 0.5, 0.5});
	return true;
}
bool daRamboo_c::collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther) {
	return false;
}
bool daRamboo_c::collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther) { 
	
	if (apOther->owner->name == 412) { // Check if it's a glow block
		dEn_c *blah = (dEn_c*)apOther->owner;
		if (blah->_12C & 3 || strcmp(blah->acState.getCurrentState()->getName(), "daLightBlock_c::StateID_Throw")) {
			return true;
		}

		SpawnEffect("Wm_en_obakedoor_sm", 0, &apOther->owner->pos, &(S16Vec){0,0,0}, &(Vec){1.0, 1.0, 1.0});
		SpawnEffect("Wm_mr_yoshistep", 0, &apOther->owner->pos, &(S16Vec){0,0,0}, &(Vec){1.0, 1.0, 1.0});

		fleeFast = true;
		doStateChange(&StateID_Flee);		
		//FIXME changed to dStageActor_c::Delete(u8) from fBase_c::Delete(void)
		apOther->owner->Delete(1);
		return true;
	}
	return false;
}
bool daRamboo_c::collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther) {
	return collisionCat1_Fireball_E_Explosion(apThis, apOther);
}
bool daRamboo_c::collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther) {
	DamagePlayer(this, apThis, apOther);
	return true;
}
bool daRamboo_c::collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther) {
	DamagePlayer(this, apThis, apOther);
	return true;
}




void daRamboo_c::bindAnimChr_and_setUpdateRates(const char* name, m3d::anmChr_c &animationChr, m3d::mdl_c &model, float rate) {
	nw4r::g3d::ResAnmChr anmChr = resFile.GetResAnmChr(name);
	animationChr.bind(&model, anmChr, 1);
	model.bindAnim(&animationChr, 0.0);
	animationChr.setUpdateRate(rate);
}


void daRamboo_c::setupModels() {
	allocator.link(-1, GameHeaps[0], 0, 0x20);

	this->resFile.data = getResource("teresa", "g3d/teresa.brres");
	bool ret;

	nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("fog");
	this->fogModel.setup(mdl, &allocator, 0x224, 1, 0);
	SetupTextures_Enemy(&this->fogModel, 0);

	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr("fog");
	ret = this->anmFog.setup(mdl, anmChr, &this->allocator, 0);


	nw4r::g3d::ResMdl mdlB = this->resFile.GetResMdl("teresaA");
	this->bodyModel.setup(mdlB, &allocator, 0x224, 1, 0);
	SetupTextures_Enemy(&this->bodyModel, 0);

	nw4r::g3d::ResAnmChr anmChrC = this->resFile.GetResAnmChr("shay_teresaA");
	ret = this->anmA.setup(mdlB, anmChrC, &this->allocator, 0);


	nw4r::g3d::ResMdl mdlC = this->resFile.GetResMdl("teresaB");
	this->hideModel.setup(mdlC, &allocator, 0x224, 1, 0);
	SetupTextures_Enemy(&this->hideModel, 0);

	nw4r::g3d::ResAnmChr anmChrE = this->resFile.GetResAnmChr("shay_teresaB");
	ret = this->anmB.setup(mdlC, anmChrE, &this->allocator, 0);

	nw4r::g3d::ResAnmTexSrt anmSrt = this->resFile.GetResAnmTexSrt("fog");
	this->resTexSrt = anmSrt;
	//setup(ResMdl mdl, ResAnmTexSrt anmSrt, mAllocator* allocator, void* NULL, int count);
	ret = this->fogSrt.setup(mdl, anmSrt, &this->allocator, 0, 1);
	//setEntryByte34(char toSet, int which);
	this->fogSrt.setEntryByte34(0, 0);

	allocator.unlink();
}



int daRamboo_c::onCreate() {

	setupModels();


	this->scale = (Vec){2.0, 2.0, 2.0};

	this->aPhysics.collisionCheckType = 1;

	ActivePhysics::Info HitMeBaby;
	HitMeBaby.xDistToCenter = 160.0;
	HitMeBaby.yDistToCenter = 80.0;

	HitMeBaby.xDistToEdge = 132.0;
	HitMeBaby.yDistToEdge = 132.0;

	HitMeBaby.category1 = 0x3;
	HitMeBaby.category2 = 0x0;
	HitMeBaby.bitfield1 = 0x4F;
	HitMeBaby.bitfield2 = 0x80222;
	HitMeBaby.unkShort1C = 0;
	HitMeBaby.callback = &dEn_c::collisionCallback;


	this->aPhysics.initWithStruct(this, &HitMeBaby);
	this->aPhysics.addToList();
	
	this->Baseline = this->pos.y;
	this->rot.x = 0; // X is vertical axis
	this->rot.y = 0xE000; // Y is horizontal axis
	this->rot.z = 0; // Z is ... an axis >.>
	this->direction = 0; // Heading left.
	this->Hiding = 1;
	this->dying = 0;
	
	this->speed.x = 0.0;
	this->ytimer = 0;
	this->pos.z = 3300.0;

	char eventNum	= (this->settings >> 16) & 0xFF;

	this->eventFlag = (u64)1 << (eventNum - 1);
		
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr("fog");
	this->anmFog.bind(&this->fogModel, anmChr, 1);
	this->fogModel.bindAnim(&this->anmFog, 0.0);
	this->anmFog.setUpdateRate(1.0);

	nw4r::g3d::ResAnmTexSrt anmSrt = this->resFile.GetResAnmTexSrt("fog");
	//bindEntry(mdl_c* model, ResAnmTexSrt anmSrt, int which, int playState?);
	this->fogSrt.bindEntry(&this->fogModel, anmSrt, 0, 1);
	this->fogModel.bindAnim(&this->fogSrt, 1.0);
	//setFrameForEntry(float frame, int which);
	this->fogSrt.setFrameForEntry(1.0, 0);
	//setUpdateRateForEntry(float rate, int which);
	this->fogSrt.setUpdateRateForEntry(1.0, 0);

	doStateChange(&StateID_Grow);

	this->onExecute();
	return true;
}

int daRamboo_c::onDelete() {
	return true;
}

int daRamboo_c::onExecute() {
	acState.execute();
	updateModelMatrices();

	this->fogModel._vf1C();

	if(this->anmFog.isAnimationDone())
		this->anmFog.setCurrentFrame(0.0);

	this->fogSrt.process();
	if(this->fogSrt.isEntryAnimationDone(0))
		this->fogSrt.setFrameForEntry(1.0, 0);
	
	if (dFlagMgr_c::instance->flags & this->eventFlag) {
		dFlagMgr_c::instance->flags = dFlagMgr_c::instance->flags && this->eventFlag;
		doStateChange(&StateID_Outro);
	}

	return true;
}

int daRamboo_c::onDraw() {
	fogModel.scheduleForDrawing();
	
	if (this->Hiding == 0) {
		bodyModel.scheduleForDrawing(); }
	else {
		hideModel.scheduleForDrawing(); }
	return true;
}


void daRamboo_c::updateModelMatrices() {
	// This won't work with wrap because I'm lazy.
	matrix.translation(pos.x + 160.0, pos.y + (scale.x * 12.0) - 80.0, pos.z);

	fogModel.setDrawMatrix(matrix);
	fogModel.setScale(&scale);
	fogModel.calcWorld(false);

	matrix.translation(pos.x + 160.0, pos.y - 80.0, pos.z + 200.0);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	if (this->Hiding == 0) {
		bodyModel.setDrawMatrix(matrix);
		bodyModel.setScale(&scale);
		bodyModel.calcWorld(false); }
	else {
		hideModel.setDrawMatrix(matrix);
		hideModel.setScale(&scale);
		hideModel.calcWorld(false); }
}


// Grow State

void daRamboo_c::beginState_Grow() { 
	this->timer = 0;
	bindAnimChr_and_setUpdateRates("begin_boss_b", anmB, hideModel, 1.0f);
	PlaySound(this, SE_EMY_TERESA);

	SetupKameck(this, Kameck);
}

void daRamboo_c::executeState_Grow() { 
	this->timer += 1;

	bool ret;
	ret = GrowBoss(this, Kameck, 1.0, 15.0, 0, this->timer);

	if (timer == 450) {
		Hiding = false;
		nw4r::snd::SoundHandle handle;
		PlaySoundWithFunctionB4(SoundRelatedClass, &handle, SE_EMY_CS_TERESA_BRING_IT, 1);
		bindAnimChr_and_setUpdateRates("begin_boss", anmA, bodyModel, 1.0f);
	}

	if (Hiding)
		hideModel._vf1C();
	else
		bodyModel._vf1C();
	if (ret && anmA.isAnimationDone()) { 	
		Hiding = 0;
		doStateChange(&StateID_Advance); 
	}
}
void daRamboo_c::endState_Grow() { 
	this->Baseline = this->pos.y;

	CleanupKameck(this, Kameck);
}



float RightmostPlayerPos() {
	dStageActor_c* current;
	current->pos.x = 0.0;

	for(char ii = 0; ii < 4; ii++) {
		dStageActor_c* player = GetSpecificPlayerActor(ii);
		if(!player) {
			continue;
		}
				// actor->pos.x, actor->pos.y, actor->pos.z,
				// player->pos.x, player->pos.y, player->pos.z);
		if(player->pos.x > current->pos.x) {
			current = player;
		}
	}
	return current->pos.x;
}


// Advance State

void daRamboo_c::beginState_Advance() { 
	this->speed.y = 0;
	this->speed.z = 0;
	this->timer = 0;

	bindAnimChr_and_setUpdateRates("wait", anmA, bodyModel, 1.0f);

}
void daRamboo_c::executeState_Advance() { 

	this->bodyModel._vf1C();

	if (this->anmA.isAnimationDone()) {
		this->anmA.setCurrentFrame(0.0); }

	float px = RightmostPlayerPos();

	if ((px - 132.0) < this->pos.x) {
		this->pos.x -= this->timer / 28.0; }
	else {
		this->pos.x = (px - 132.0); }

	this->pos.y = this->Baseline + sin(this->ytimer * 3.14 / 192) * 36;
	
		
	if (this->timer >= 32) { this->timer = 31; }
	if (this->ytimer >= 384) { this->ytimer = 0; }
	
	PlaySound(this, SE_EMY_TERESA);
	
	this->timer += 1;
	this->ytimer += 1;
}

void daRamboo_c::endState_Advance() {  }





// Wait State

void daRamboo_c::beginState_Wait() { 

	this->timer = 0;
	PlaySound(this, SE_EMY_TERESA_STOP);

}
void daRamboo_c::executeState_Wait() { 

	if (this->timer < 90) {
		this->hideModel._vf1C(); }

	if (this->timer > 55) {
		this->bodyModel._vf1C(); }
	
	
	if (this->timer == 70)  {
		this->Hiding = 0;
	
		bindAnimChr_and_setUpdateRates("shay_teresaA", anmA, bodyModel, 1.0f);
	}
			

	if (this->timer == 55)  {
		bindAnimChr_and_setUpdateRates("shay_teresaB", anmB, hideModel, 1.0f);
	}		



	if (this->anmB.isAnimationDone()) { 
		PlaySound(this, SE_EMY_CS_TERESA_BEAT_YOU);
		doStateChange(&StateID_Advance); 
	}

	this->timer += 1;

}

void daRamboo_c::endState_Wait() { 
	
	this->Hiding = 0;

	bindAnimChr_and_setUpdateRates("shay_teresaA", anmA, bodyModel, 1.0f);
}





// Flee State

void daRamboo_c::beginState_Flee() { 

	this->timer = 0;

}
void daRamboo_c::executeState_Flee() { 
	
	this->hideModel._vf1C(); 

	if (this->timer == 10) {
		this->Hiding = 1;
	
		bindAnimChr_and_setUpdateRates("shay_teresaB_wait", anmB, hideModel, 1.0f);
	}
	
	this->pos.x += (60 - this->timer) / (fleeFast ? 8 : 25);
	

	if ((this->timer > 60) && (this->anmB.isAnimationDone())) { 
		doStateChange(&StateID_Wait); 
	}
	
	this->timer += 1;
}

void daRamboo_c::endState_Flee() { 
}




void daRamboo_c::beginState_Outro() { 

	bindAnimChr_and_setUpdateRates("DEATH", anmB, hideModel, 1.0f);
	Hiding = true;

	this->timer = 0;
	this->rot.x = 0x0; // X is vertical axis
	this->rot.y = 0xE000; // Y is horizontal axis
	this->rot.z = 0x0; // Z is ... an axis >.>

	OutroSetup(this);
}
void daRamboo_c::executeState_Outro() {
	hideModel._vf1C();
	if (this->anmB.isAnimationDone())
		this->anmB.setCurrentFrame(0.0);

	if (this->dying == 1) { 
		if (this->timer > 180) {
			ExitStage(WORLD_MAP, 0, BEAT_LEVEL, MARIO_WIPE);
		}
		
		if (this->timer == 60) {
			PlayerVictoryCries(this);
		}	
		
		this->timer = timer + 1;
		return; 
	}	

	bool ret;
	Vec tempPos = (Vec){this->pos.x + 160.0, this->pos.y + 80.0, 5500.0};
	ret = ShrinkBoss(this, &tempPos, 0.0, this->timer);

	if (timer >= 118) { 
		BossExplode(this, &tempPos); 
		this->dying = 1;
		this->timer = 0;	
	}
	else if (timer == 20) {
		nw4r::snd::SoundHandle handle;
		PlaySoundWithFunctionB4(SoundRelatedClass, &handle, SE_EMY_BIG_TERESA_DEAD, 1);
	}

	timer = timer + 1;
}
void daRamboo_c::endState_Outro() { }



//
// processed\../src/bossBalboaWrench.cpp
//

#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include <stage.h>
#include "boss.h"

class daBalboa_c : public daBoss {
	int onCreate();
	int onDelete();
	int onExecute();
	int onDraw();

	mHeapAllocator_c allocator;
	m3d::mdl_c bodyModel;
	m3d::mdl_c spikesModel;

	nw4r::g3d::ResFile resFile;
	m3d::anmChr_c animationChr;

	int timer;
	int damage;
	float Baseline;
	float dying;
	Vec PopUp [3]; 
	char throwCount;
	char throwMax;
	float throwRate;
	char upsideDown;
	int isBigBoss;
	char isRevenging;
	int spinner;

	ActivePhysics spikeCollision;
	float spikeOffset;
	bool spikeGoingUp, spikeGoingDown;
	
	static daBalboa_c *build();

	void setupModels();
	void updateModelMatrices();
	void bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate);

	bool prePlayerCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	void playerCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther);

	void addScoreWhenHit(void *other);
	
	USING_STATES(daBalboa_c);
	DECLARE_STATE(Grow);
	DECLARE_STATE(ManholeUp);
	DECLARE_STATE(HeadPoke);
	DECLARE_STATE(AllOut);
	DECLARE_STATE(ThrowWrench);
	DECLARE_STATE(BackDown);
	DECLARE_STATE(Outro);
	DECLARE_STATE(Damage);
	DECLARE_STATE(RevengeUp);
	DECLARE_STATE(Revenge);
};

daBalboa_c *daBalboa_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daBalboa_c));
	return new(buffer) daBalboa_c;
}

// Externs

	CREATE_STATE(daBalboa_c, Grow);
	CREATE_STATE(daBalboa_c, ManholeUp);
	CREATE_STATE(daBalboa_c, HeadPoke);
	CREATE_STATE(daBalboa_c, AllOut);
	CREATE_STATE(daBalboa_c, ThrowWrench);
	CREATE_STATE(daBalboa_c, BackDown);
	CREATE_STATE(daBalboa_c, Outro);
	CREATE_STATE(daBalboa_c, Damage);
	CREATE_STATE(daBalboa_c, RevengeUp);
	CREATE_STATE(daBalboa_c, Revenge);

// Collisions
	void balbieCollisionCallback(ActivePhysics *apThis, ActivePhysics *apOther);	

	void balbieCollisionCallback(ActivePhysics *apThis, ActivePhysics *apOther) {
		if (apOther->owner->name != 544) { 
			dEn_c::collisionCallback(apThis, apOther); 
		}
	}

	void daBalboa_c::addScoreWhenHit(void *other) {}

bool daBalboa_c::prePlayerCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
	if (apOther->owner->stageActorType == 1) {
		if (apOther->info.category2 == 7) {
			if (collisionCat7_GroundPound(apThis, apOther))
				return true;
		}
	}

	return dEn_c::prePlayerCollision(apThis, apOther);
}

	void daBalboa_c::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther) { 

		char ret = usedForDeterminingStatePress_or_playerCollision(this, apThis, apOther, 0);

		if(ret == 0) {
			this->dEn_c::playerCollision(apThis, apOther);
			this->_vf220(apOther->owner);
		}

		//FIXME hack to make multiple playerCollisions work
		deathInfo.isDead = 0;
		this->flags_4FC |= (1<<(31-7));
		this->counter_504[apOther->owner->which_player] = 0;
	}

	bool daBalboa_c::collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther) { 

		dActor_c *block = apOther->owner;
		dEn_c *mario = (dEn_c*)block;

		SpawnEffect("Wm_en_vshit", 0, &mario->pos, &(S16Vec){0,0,0}, &(Vec){1.0, 1.0, 1.0});

		mario->speed.y = -mario->speed.y;
		mario->pos.y += mario->speed.y;

		if (mario->direction == 0) { mario->speed.x = 4.0; }
		else					  { mario->speed.x = -4.0; }
		
		mario->doSpriteMovement();
		mario->doSpriteMovement();

		if (isRevenging) {
			_vf220(apOther->owner);
		} else {
			this->damage -= 1;
			
			apOther->someFlagByte |= 2;
			
			PlaySoundAsync(this, SE_EMY_PENGUIN_DAMAGE);			

			doStateChange(&StateID_Damage);
		}

		return true;
	}
	bool daBalboa_c::collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther) { 
		return collisionCat7_GroundPound(apThis, apOther);
	}

	bool daBalboa_c::collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther) {
		return true;
	}
	bool daBalboa_c::collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther) {
		return false;
	}
	bool daBalboa_c::collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther) {
		return true;
	}
	bool daBalboa_c::collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther) {
		return true;
	}
	bool daBalboa_c::collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther) {
		dAcPy_vf3F8(apOther->owner, this, 3);
		return true;
	}


void daBalboa_c::setupModels() {
	allocator.link(-1, GameHeaps[0], 0, 0x20);

	nw4r::g3d::ResMdl mdl;
	nw4r::g3d::ResAnmChr anmChr;

	this->resFile.data = getResource("choropoo", "g3d/choropoo.brres");

	mdl = this->resFile.GetResMdl("choropoo");
	this->bodyModel.setup(mdl, &allocator, 0x224, 1, 0);
	SetupTextures_Enemy(&this->bodyModel, 0);

	anmChr = this->resFile.GetResAnmChr("throw_1"); // 11
	this->animationChr.setup(mdl, anmChr, &this->allocator, 0);

	nw4r::g3d::ResFile togeRes;
	togeRes.data = getResource("lift_zen", "g3d/lift_zen.brres");
	mdl = togeRes.GetResMdl("lift_togeU");
	spikesModel.setup(mdl, &allocator, 0, 1, 0);

	// throw_1 // 11
	// throw_2 // 75
	// throw_3 // 33
	// throw_4_left_hand // 87
	// throw_4_right_hand // 87
	// throw_5 // 23

	allocator.unlink();
}

// Animation Order... 
// AppearLittle - Throw One, sound 0x21F
// Search - Throw two
// AppearFull - Throw 3 and sound 0x220
// Attack - Throw 4
// Disappear - Throw 5


void daBalboa_c::bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate) {
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr(name);
	this->animationChr.bind(&this->bodyModel, anmChr, unk);
	this->bodyModel.bindAnim(&this->animationChr, unk2);
	this->animationChr.setUpdateRate(rate);
}

int daBalboa_c::onCreate() {

	setupModels();

	this->scale = (Vec){1.0, 1.0, 1.0};
	this->isBigBoss = (this->settings >> 28);

	ActivePhysics::Info HitMeBaby;
	HitMeBaby.xDistToCenter = 0.0;
	HitMeBaby.yDistToCenter = 27.0;

	HitMeBaby.xDistToEdge = 18.0;
	HitMeBaby.yDistToEdge = 24.0;

	HitMeBaby.category1 = 0x3;
	HitMeBaby.category2 = 0x0;
	HitMeBaby.bitfield1 = 0x4F;
	HitMeBaby.bitfield2 = 0xFFBAFFFE;
	HitMeBaby.unkShort1C = 0;
	HitMeBaby.callback = &balbieCollisionCallback;


	this->aPhysics.initWithStruct(this, &HitMeBaby);
	this->aPhysics.addToList();

	ActivePhysics::Info spikeInfo = {
		0.0f, 0.0f, 16.0f, 16.0f,
		3, 0, 0x4F, 0xFFBAFFFE, 0, &dEn_c::collisionCallback};
	spikeCollision.initWithStruct(this, &spikeInfo);
	spikeCollision.trpValue0 = 0.0f;
	spikeCollision.trpValue1 = 0.0f;
	spikeCollision.trpValue2 = -16.0f;
	spikeCollision.trpValue3 = 16.0f;
	spikeCollision.collisionCheckType = 3;

	this->rot.x = 0; // X is vertical axis
	this->rot.y = 0xE000; // Y is horizontal axis
	this->rot.z = 0; // Z is ... an axis >.>
	this->upsideDown = 0;
	this->direction = 0; // Heading left.
	this->pos.z = -800.0;
	this->pos.y -= 8.0;
	this->damage = 3;
	this->isRevenging = 0;

	this->PopUp[0] = (Vec){this->pos.x, this->pos.y - 54.0, this->pos.z};
	this->PopUp[1] = (Vec){this->pos.x - 224.0, this->pos.y - 54.0, this->pos.z};
	this->PopUp[2] = (Vec){this->pos.x - 112.0, this->pos.y - 22.0, this->pos.z};
	this->PopUp[3] = (Vec){this->pos.x - 112.0, this->pos.y - 22.0, this->pos.z};


	doStateChange(&StateID_Grow);

	this->onExecute();
	return true;
}

int daBalboa_c::onDelete() {
	return true;
}

int daBalboa_c::onExecute() {
	acState.execute();
	updateModelMatrices();

	if (spikeGoingUp) {
		spikeOffset += 2.5f;
		if (spikeOffset >= 48.0f) {
			spikeOffset = 48.0f;
			spikeGoingUp = false;
		}
	} else if (spikeGoingDown) {
		spikeOffset -= 2.5f;
		if (spikeOffset <= 0.0f) {
			spikeOffset = 0.0f;
			spikeGoingDown = false;
		}
	}
	spikeCollision.info.yDistToCenter = 16.0f + spikeOffset;
	
	bodyModel._vf1C();
		
	return true;
}

int daBalboa_c::onDraw() {
	
	bodyModel.scheduleForDrawing(); 
	if (spikeOffset > 0.0f)
		spikesModel.scheduleForDrawing();

	return true;
}

void daBalboa_c::updateModelMatrices() {
	// This won't work with wrap because I'm lazy.
	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	bodyModel.setDrawMatrix(matrix);
	bodyModel.setScale(&scale);
	bodyModel.calcWorld(false); 

	mMtx spikeMatrix;
	VEC3 spikeScale = {2.0f,1.8f,2.0f};
	spikeMatrix.translation(pos.x, pos.y + spikeOffset, pos.z);
	spikesModel.setDrawMatrix(spikeMatrix);
	spikesModel.setScale(&spikeScale);
	spikesModel.calcWorld(false);
}

// Grow State

	void daBalboa_c::beginState_Grow() { 
		this->timer = 0;

		SetupKameck(this, Kameck);

		bindAnimChr_and_setUpdateRate("begin_boss", 1, 0.0, 0.6); 
	}

	void daBalboa_c::executeState_Grow() { 

		if(this->animationChr.isAnimationDone())
			this->animationChr.setCurrentFrame(0.0);

		this->timer += 1;
		
		bool ret;
		ret = GrowBoss(this, Kameck, 1.0, 2.25, 0, this->timer);

		if (ret) { 	
			PlaySound(this, SE_EMY_CHOROPU_BOUND);
			doStateChange(&StateID_BackDown); 
		}
	}
	void daBalboa_c::endState_Grow() { 
		CleanupKameck(this, Kameck);
	}


// ManholeUp State

	void daBalboa_c::beginState_ManholeUp() { 

		bindAnimChr_and_setUpdateRate("throw_1", 1, 0.0, 1.0); 

		this->timer = 0;
			
		int randChoice;
		randChoice = GenerateRandomNumber(3);
		
		this->pos = this->PopUp[randChoice];


		
		if 		(randChoice == 0) { // On the left side!
			this->rot.y = 0xE000; 
			this->rot.z = 0;
			this->upsideDown = 0;
			this->direction = 0; }

		else if (randChoice == 1) {	// On the right side!
			this->rot.y = 0x2000; 
			this->rot.z = 0;
			this->upsideDown = 0;
			this->direction = 1; }

		// else if (randChoice == 2) {	// On the right ceiling!
		// 	this->rot.y = 0xE000; 
		// 	this->rot.z = 0x8000;
		// 	this->upsideDown = 1;
		// 	this->direction = 0; }

		// else if (randChoice == 3) {	// On the left ceiling!
		// 	this->rot.y = 0x2000; 
		// 	this->rot.z = 0x8000;
		// 	this->upsideDown = 1;
		// 	this->direction = 1; }

		else if (randChoice == 2) {	// In the Center!
			char Pdir = dSprite_c__getXDirectionOfFurthestPlayerRelativeToVEC3(this, this->pos);

			if (Pdir == 1) {
				this->rot.y = 0xE000; 
				this->direction = 0; }
			else {
				this->rot.y = 0x2000; 
				this->direction = 1; }
		}

		PlaySound(this, 0x21F);
	}

	void daBalboa_c::executeState_ManholeUp() { 

		if (this->timer > 51) {
			doStateChange(&StateID_HeadPoke); 
		}
		if (this->timer > 11) { }
		else {	
			this->pos.y += 0.8182; // Height is 54 pixels, move up 9 pixels.
		}
			
		this->timer += 1;		
	}
	void daBalboa_c::endState_ManholeUp() { }


// HeadPoke State

	void daBalboa_c::beginState_HeadPoke() { 

		bindAnimChr_and_setUpdateRate("throw_2", 1, 0.0, 1.0); 

		this->timer = 0;
	}

	void daBalboa_c::executeState_HeadPoke() { 

		this->pos.y += 0.24; // Height is 54 pixels, move up 18 pixels.
				
		if(this->animationChr.isAnimationDone()) {
			doStateChange(&StateID_AllOut); }

	}
	void daBalboa_c::endState_HeadPoke() { }


// AllOut State

	void daBalboa_c::beginState_AllOut() { 

		bindAnimChr_and_setUpdateRate("throw_3", 1, 0.0, 1.0); 

		this->timer = 0;

		PlaySound(this, 0x220);
	}

	void daBalboa_c::executeState_AllOut() { 


		this->pos.y += 0.8182; // Height is 54 pixels, move up 27 pixels.
				
		if(this->animationChr.isAnimationDone()) {
			doStateChange(&StateID_ThrowWrench); 
		}
	}
	void daBalboa_c::endState_AllOut() { }


// ThrowWrench State

	void daBalboa_c::beginState_ThrowWrench() { 

		this->throwCount = 0;
		if (this->isBigBoss == 1) {
			throwMax = 6;
			throwRate = 3.0;
		}
		else {
			throwMax = 4;
			throwRate = 2.0;
		}
		bindAnimChr_and_setUpdateRate("throw_4_right_hand", 1, 0.0, throwRate);
	}

	void daBalboa_c::executeState_ThrowWrench() { 

		float frame = this->animationChr.getCurrentFrame();
		if (frame == 54.0) {
			u32 settings;
			u8 up = this->upsideDown;
			u8 throwc = this->throwCount;
			u8 dir;
			
			if (this->direction) { dir = 0; }
			else 				 { dir = 1; }

			settings = (dir) | (up << 1);
			settings = settings | (throwc & 1 << 8);

			if (this->isBigBoss == 1) { settings = settings | 0x10; }

			CreateActor(544, settings, this->pos, 0, 0);
		}

		if(this->animationChr.isAnimationDone()) { 
			this->throwCount += 1;

			if (this->throwCount & 1) {
				bindAnimChr_and_setUpdateRate("throw_4_left_hand", 1, 0.0, throwRate); 
			}
			else {
				bindAnimChr_and_setUpdateRate("throw_4_right_hand", 1, 0.0, throwRate); 
			}
		}
		
		if (this->throwCount > throwMax) {
			doStateChange(&StateID_BackDown); }		
	}
	void daBalboa_c::endState_ThrowWrench() { }


// BackDown State

	void daBalboa_c::beginState_BackDown() { 

		bindAnimChr_and_setUpdateRate("throw_5", 1, 0.0, 1.0); 

		this->timer = 0;

		PlaySound(this, 0x221);

		SpawnEffect("Wm_mr_sanddive_out", 0, &this->pos, &(S16Vec){0,0,0}, &(Vec){2.0, 1.0, 1.0});
		SpawnEffect("Wm_mr_sanddive_smk", 0, &this->pos, &(S16Vec){0,0,0}, &(Vec){2.0, 1.0, 1.0});
	}

	void daBalboa_c::executeState_BackDown() { 

		if (this->timer < 60) {
			this->pos.y -= 2.0;  // Height is 54 pixels, move down 
		}
			
		if (this->timer > 90) {
			doStateChange(&StateID_ManholeUp); }
			
		this->timer += 1;

	}
	void daBalboa_c::endState_BackDown() { }


// Outro 

	void daBalboa_c::beginState_Outro() { 

		bindAnimChr_and_setUpdateRate("dead", 1, 0.0, 1.0); 

		OutroSetup(this);
		this->timer = 0;
		this->rot.x = 0x0; // X is vertical axis
		this->rot.z = 0x0; // Z is ... an axis >.>

	}
	void daBalboa_c::executeState_Outro() {

		if(this->animationChr.isAnimationDone())
			this->animationChr.setCurrentFrame(0.0);

		if (this->dying == 1) { 
			if (this->timer > 180) { ExitStage(WORLD_MAP, 0, BEAT_LEVEL, MARIO_WIPE); }
			if (this->timer == 60) { PlayerVictoryCries(this); }	
			
			this->timer += 1;
			return; 
		}	

		bool ret;
		ret = ShrinkBoss(this, &this->pos, 2.25, this->timer);
		this->pos.y -= 0.02;

		if (ret == true) 	{ 
			BossExplode(this, &this->pos); 
			this->dying = 1;
			this->timer = 0;	
		}
		else 		{ PlaySound(this, SE_EMY_CHOROPU_SIGN); }
	
		this->timer += 1;

	}
	void daBalboa_c::endState_Outro() { }


// Damage

	void daBalboa_c::beginState_Damage() { 
		
		bindAnimChr_and_setUpdateRate("dead", 1, 0.0, 0.5); 

		this->timer = 0;
		this->removeMyActivePhysics();
	}
	void daBalboa_c::executeState_Damage() {

		if (this->timer > 6) { doStateChange(&StateID_RevengeUp); }

		if(this->animationChr.isAnimationDone()) { 
			this->animationChr.setCurrentFrame(0.0);

			this->timer += 1;
		}

		if (this->timer > 3) {
			this->pos.y -= 5.0; // Height is 54 pixels, move down 
		}
		else if (this->timer > 2) {
			if (this->damage == 0) { 
				StopBGMMusic();
				doStateChange(&StateID_Outro); 
			}
			this->pos.y -= 3.5; // Height is 54 pixels, move down 
		}
		else if (this->timer > 1) {
			this->pos.y -= 1.0; // Height is 54 pixels, move down 
		}
		else if (this->timer > 0) {
			this->pos.y += 1.0; // Height is 54 pixels, move down 
		}
		else {
			this->pos.y += 3.5; // Height is 54 pixels, move down 
		}

	}
	void daBalboa_c::endState_Damage() { 
		this->addMyActivePhysics();
	}


// Revenge Up

	void daBalboa_c::beginState_RevengeUp() { 

		this->pos = this->PopUp[2];
		this->rot.y = 0;
		
		isRevenging = 1;
		bindAnimChr_and_setUpdateRate("throw_3", 1, 0.0, 1.0); 
		spikeGoingUp = true;
		spikeCollision.addToList();

		PlaySound(this, 0x220);
	}
	void daBalboa_c::executeState_RevengeUp() {

		this->pos.y += 1.6363; // Height is 54 pixels, move up 27 pixels.
				
		if(this->animationChr.isAnimationDone()) {
			doStateChange(&StateID_Revenge); 
		}

	}
	void daBalboa_c::endState_RevengeUp() { }


// Revenge

	void daBalboa_c::beginState_Revenge() { 
		spinner = 0;

		this->throwCount = 0;
		if (this->isBigBoss == 1) {
			throwMax = 16;
			throwRate = 5.0;
		}
		else {
			throwMax = 12;
			throwRate = 3.0;
		}
		bindAnimChr_and_setUpdateRate("throw_4_right_hand", 1, 0.0, throwRate);
	}
	void daBalboa_c::executeState_Revenge() {

		float frame = this->animationChr.getCurrentFrame();

		rot.y = 16384.0 * (frame / 87.0) + (16384.0 * spinner);

		if (frame == 60.0) {
			u32 settings;
			u8 up = this->upsideDown;
			u8 throwc = this->throwCount;
			u8 dir;
			
			if (spinner < 2)	 { dir = 0; }
			else 				 { dir = 1; }

			settings = (dir) | (up << 1);
			settings = settings | (throwc & 1 << 8);

			if (this->isBigBoss == 1) { settings = settings | 0x10; }

			CreateActor(544, settings, this->pos, 0, 0);
		}

		if(this->animationChr.isAnimationDone()) { 
			this->throwCount += 1;
			spinner += 1;
			if (spinner == 4) { spinner = 0; }

			if (this->throwCount & 1) {
				bindAnimChr_and_setUpdateRate("throw_4_left_hand", 1, 0.0, throwRate); 
			}
			else {
				bindAnimChr_and_setUpdateRate("throw_4_right_hand", 1, 0.0, throwRate); 
			}
		}
		
		if (this->throwCount > throwMax) {
			doStateChange(&StateID_BackDown); }

	}
	void daBalboa_c::endState_Revenge() { 
			isRevenging = 0;
			spikeGoingDown = true;
			spikeCollision.removeFromList();
	}

//
// processed\../src/bossWrenchThrow.cpp
//

#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include "boss.h"

class daWrench : public dEn_c {
	int onCreate();
	int onExecute();
	int onDelete();
	int onDraw();

	mHeapAllocator_c allocator;
	m3d::mdl_c bodyModel;

	int timer;
	char Kaboom;
	char direction;
	char front;
	float ymod;
	int lifespan;
	u32 cmgr_returnValue;

	static daWrench *build();

	void updateModelMatrices();
	void playerCollision(ActivePhysics *apThis, ActivePhysics *apOther);

	bool collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther);


	USING_STATES(daWrench);
	DECLARE_STATE(Straight);
	DECLARE_STATE(Kaboom);

};

CREATE_STATE(daWrench, Straight);
CREATE_STATE(daWrench, Kaboom);


extern "C" void *PlayWrenchSound(dEn_c *);


void daWrench::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther) { DamagePlayer(this, apThis, apOther); }

bool daWrench::collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther) {
	return true;
}
bool daWrench::collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther) { 
	return false; 
}
bool daWrench::collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther) {
	return true;
}
bool daWrench::collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther) {
	SpawnEffect("Wm_ob_cmnboxgrain", 0, &this->pos, &(S16Vec){0,0,0}, &(Vec){0.5, 0.5, 0.5});

	PlaySoundAsync(this, SE_BOSS_JR_FLOOR_BREAK);
	this->Delete(1);
	return true;
}
bool daWrench::collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther) {
	return true;
}
bool daWrench::collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther) { 
	this->_vf220(apOther->owner);

	SpawnEffect("Wm_ob_cmnboxgrain", 0, &this->pos, &(S16Vec){0,0,0}, &(Vec){0.5, 0.5, 0.5});

	PlaySoundAsync(this, SE_BOSS_JR_FLOOR_BREAK);
	this->Delete(1);
	return true;
}



daWrench *daWrench::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daWrench));
	return new(buffer) daWrench;
}


int daWrench::onCreate() {
	
	allocator.link(-1, GameHeaps[0], 0, 0x20);

	nw4r::g3d::ResFile rf(getResource("choropoo", "g3d/choropoo.brres"));
	bodyModel.setup(rf.GetResMdl("spanner"), &allocator, 0x224, 1, 0);
	SetupTextures_Enemy(&bodyModel, 0);

	allocator.unlink();
	
	
	this->direction = this->settings & 0xF;
	this->Kaboom = (this->settings >> 4) & 0xF;
	this->front = (this->settings >> 8) & 0xF;
	
	ActivePhysics::Info HitMeBaby;
	
	if (this->Kaboom == 0) {
		HitMeBaby.xDistToCenter = 0.0;
		HitMeBaby.yDistToCenter = 0.0;
		HitMeBaby.xDistToEdge = 5.0;
		HitMeBaby.yDistToEdge = 5.0;
	
		this->scale.x = 1.25;
		this->scale.y = 1.25;
		this->scale.z = 1.25;
	}
	
	else {
		HitMeBaby.xDistToCenter = 0.0;
		HitMeBaby.yDistToCenter = 0.0;
		HitMeBaby.xDistToEdge = 8.0;
		HitMeBaby.yDistToEdge = 8.0;
	
		this->scale.x = 2.0;
		this->scale.y = 2.0;
		this->scale.z = 2.0;
	}
	
	HitMeBaby.category1 = 0x3;
	HitMeBaby.category2 = 0x0;
	HitMeBaby.bitfield1 = 0x47;
	HitMeBaby.bitfield2 = 0xFFFFFFFF;
	HitMeBaby.unkShort1C = 0;
	HitMeBaby.callback = &dEn_c::collisionCallback;

	this->aPhysics.initWithStruct(this, &HitMeBaby);
	this->aPhysics.addToList();


	spriteSomeRectX = 5.0f;
	spriteSomeRectY = 5.0f;
	_320 = 0.0f;
	_324 = 5.0f;

	// These structs tell stupid collider what to collide with - these are from koopa troopa
	static const lineSensor_s below(12<<12, 4<<12, 0<<12);
	static const pointSensor_s above(0<<12, 12<<12);
	static const lineSensor_s adjacent(6<<12, 9<<12, 6<<12);

	collMgr.init(this, &below, &above, &adjacent);
	collMgr.calculateBelowCollisionWithSmokeEffect();

	cmgr_returnValue = collMgr.isOnTopOfTile();


	if (this->direction == 0) 	   { // Ground Facing Left
		this->pos.x -= 0.0; // -32 to +32
		this->pos.y += 36.0;
		this->rot.z = 0x2000;
	}
	else if (this->direction == 1) { // Ground Facing Right
		this->pos.x += 0.0; // +32 to -32
		this->pos.y += 36.0;
		this->rot.z = 0xE000;
	}
	if (this->front == 1) { this->pos.z = -1804.0; }
	else 				  { this->pos.z =  3300.0; }

	
	if (this->Kaboom) {
		doStateChange(&StateID_Kaboom); }
	else {
		doStateChange(&StateID_Straight); }
	
	this->onExecute();
	return true;
}


int daWrench::onDelete() {
	return true;
}

int daWrench::onDraw() {
	bodyModel.scheduleForDrawing();
	return true;
}


void daWrench::updateModelMatrices() {
	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	bodyModel.setDrawMatrix(matrix);
	bodyModel.setScale(&scale);
	bodyModel.calcWorld(false);
}


int daWrench::onExecute() {
	acState.execute();
	updateModelMatrices();

	float rect[] = {this->_320, this->_324, this->spriteSomeRectX, this->spriteSomeRectY};
	int ret = this->outOfZone(this->pos, (float*)&rect, this->currentZoneID);
	if(ret) {
		this->Delete(1);
	}

	return true;
}


void daWrench::beginState_Kaboom() { 
	float rand = (float)GenerateRandomNumber(10) * 0.4;

	if (this->direction == 0) { // directions 1 spins clockwise, fly rightwards
		speed.x = 1.0 + rand; 
	}
	else {						// directions 0 spins anti-clockwise, fly leftwards
		speed.x = -1.0 - rand; 
	}

	speed.y = 6.0;
}
void daWrench::executeState_Kaboom() { 

	speed.y = speed.y - 0.01875;

	HandleXSpeed();
	HandleYSpeed();
	doSpriteMovement();

	cmgr_returnValue = collMgr.isOnTopOfTile();
	collMgr.calculateBelowCollisionWithSmokeEffect();

	if (collMgr.isOnTopOfTile()) {
		// hit the ground
		PlaySoundAsync(this, SE_BOSS_JR_BOMB_BURST);

		SpawnEffect("Wm_en_burst_s", 0, &this->pos, &(S16Vec){0,0,0}, &(Vec){0.75, 0.75, 0.75});
		SpawnEffect("Wm_mr_wirehit", 0, &this->pos, &(S16Vec){0,0,0}, &(Vec){1.25, 1.25, 1.25});
		this->Delete(1);
	}
	if (collMgr.outputMaybe & (0x15 << direction)) {
		// hit the wall
		PlaySoundAsync(this, SE_BOSS_JR_BOMB_BURST);
 
		if (this->direction == 0) { // directions 1 spins clockwise, fly rightwards
			SpawnEffect("Wm_en_burst_s", 0, &this->pos, &(S16Vec){0,0x4000,0}, &(Vec){0.75, 0.75, 0.75});
			SpawnEffect("Wm_mr_wirehit", 0, &this->pos, &(S16Vec){0,0x4000,0}, &(Vec){1.25, 1.25, 1.25});
		}
		else {						// directions 0 spins anti-clockwise, fly leftwards
			SpawnEffect("Wm_en_burst_s", 0, &this->pos, &(S16Vec){0,0xE000,0}, &(Vec){0.75, 0.75, 0.75});
			SpawnEffect("Wm_mr_wirehit", 0, &this->pos, &(S16Vec){0,0xE000,0}, &(Vec){1.25, 1.25, 1.25});
		}

		this->Delete(1);
	}

	if (this->direction == 1) { // directions 1 spins clockwise, fly rightwards
		this->rot.z -= 0x1000; }
	else {						// directions 0 spins anti-clockwise, fly leftwards
		this->rot.z += 0x1000; }	
	
	PlayWrenchSound(this);

}
void daWrench::endState_Kaboom() { }



void daWrench::beginState_Straight() { 
	float rand = (float)GenerateRandomNumber(10) * 0.4;

	if (this->direction == 0) { // directions 1 spins clockwise, fly rightwards
		speed.x = 1.0 + rand; 
	}
	else {						// directions 0 spins anti-clockwise, fly leftwards
		speed.x = -1.0 - rand; 
	}

	speed.y = 6.0;
}
void daWrench::executeState_Straight() { 

	speed.y = speed.y - 0.01875;

	HandleXSpeed();
	HandleYSpeed();
	doSpriteMovement();

	cmgr_returnValue = collMgr.isOnTopOfTile();
	collMgr.calculateBelowCollisionWithSmokeEffect();

	if (collMgr.isOnTopOfTile()) {
		// hit the ground
		PlaySoundAsync(this, SE_BOSS_JR_FLOOR_BREAK);

		SpawnEffect("Wm_en_burst_s", 0, &this->pos, &(S16Vec){0,0,0}, &(Vec){0.75, 0.75, 0.75});
		this->Delete(1);
	}
	if (collMgr.outputMaybe & (0x15 << direction)) {
		// hit the wall
		PlaySoundAsync(this, SE_BOSS_JR_FLOOR_BREAK);

		SpawnEffect("Wm_en_burst_s", 0, &this->pos, &(S16Vec){0,0,0}, &(Vec){0.75, 0.75, 0.75});
		this->Delete(1);
	}

	if (this->direction == 1) { // directions 1 spins clockwise, fly rightwards
		this->rot.z -= 0x1000; }
	else {						// directions 0 spins anti-clockwise, fly leftwards
		this->rot.z += 0x1000; }	
	
	PlayWrenchSound(this);

}
void daWrench::endState_Straight() { }










//
// processed\../src/bossSamurshai.cpp
//

#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include "boss.h"

extern "C" void *SoundRelatedClass;
extern "C" void *MapSoundPlayer(void *SoundRelatedClass, int soundID, int unk);


const char* SSarcNameList [] = {
	"Shynja",
	NULL	
};

void ChucksAndKnucks(ActivePhysics *apThis, ActivePhysics *apOther);

class daSamurshai : public daBoss {
	int onCreate();
	int onDelete();
	int onExecute();
	int onDraw();

	mHeapAllocator_c allocator;
	nw4r::g3d::ResFile resFile;
	m3d::mdl_c bodyModel;
	m3d::anmChr_c chrAnimation;

	mEf::es2 effect;

	int isBigBoss;
	int jumpCounter;
	char isDown;
	Vec initialPos;
	float XSpeed;
	u32 cmgr_returnValue;
	dStageActor_c *chosenOne;
	bool topHurts;
	bool slowDown;
	bool isBouncing;
	bool walkStateIsCharging;
	float amountCharged;

	ActivePhysics Chuckles;
	ActivePhysics Knuckles;

	static daSamurshai *build();

	void bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate);
	void updateModelMatrices();
	bool calculateTileCollisions();

	void powBlockActivated(bool isNotMPGP);

	void playerCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	void yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther);

	bool collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther);

	void addScoreWhenHit(void *other);
	int randomPlayer();

	bool isNearWall();

	USING_STATES(daSamurshai);

	DECLARE_STATE(Intro);
	DECLARE_STATE(Walk);
	DECLARE_STATE(Turn);
	DECLARE_STATE(Chop);
	DECLARE_STATE(ChargeSlash);
	DECLARE_STATE(Uppercut);
	DECLARE_STATE(SpinAttack);
	DECLARE_STATE(Damage);
	DECLARE_STATE(Outro);
};

daSamurshai *daSamurshai::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daSamurshai));
	return new(buffer) daSamurshai;
}

///////////////////////
// Externs and States
///////////////////////
	extern "C" int SmoothRotation(short* rot, u16 amt, int unk2);

	CREATE_STATE(daSamurshai, Intro);
	CREATE_STATE(daSamurshai, Walk);
	CREATE_STATE(daSamurshai, Turn);
	CREATE_STATE(daSamurshai, Chop);
	CREATE_STATE(daSamurshai, ChargeSlash);
	CREATE_STATE(daSamurshai, Uppercut);
	CREATE_STATE(daSamurshai, SpinAttack);
	CREATE_STATE(daSamurshai, Damage);
	CREATE_STATE(daSamurshai, Outro);


////////////////////////
// Collision Functions
////////////////////////

	void ChucksAndKnucks(ActivePhysics *apThis, ActivePhysics *apOther) {
		if (((dEn_c*)apOther->owner)->name == PLAYER)
			((dEn_c*)apThis->owner)->_vf220(apOther->owner);
	}

	void daSamurshai::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther) {

		char hitType = 0;
		// this is shit code
		dStateBase_c *whatState = acState.getCurrentState();
		if (whatState == &StateID_Damage) {
			// nothing
		} else if (whatState == &StateID_Walk && walkStateIsCharging) {
			// also nothing
		} else {
			hitType = usedForDeterminingStatePress_or_playerCollision(this, apThis, apOther, 2);
		}

		if (hitType == 2) {
			// Mini jump
			apOther->someFlagByte |= 2;
		} else if (hitType > 0) {
			apOther->someFlagByte |= 2;
			if (this->isDown == 0) { 
				this->playEnemyDownSound1();
				damage += 5;
				if (damage >= 15) { doStateChange(&StateID_Outro); }
				else { doStateChange(&StateID_Damage); }
			}
		} 
		else {
			this->dEn_c::playerCollision(apThis, apOther);
			this->_vf220(apOther->owner);
		} 

		deathInfo.isDead = 0;
		this->flags_4FC |= (1<<(31-7));
		this->counter_504[apOther->owner->which_player] = 0;
	}

	void daSamurshai::yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther) { this->playerCollision(apThis, apOther); }
	bool daSamurshai::collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther) {
		return collisionCat7_GroundPound(apThis, apOther);
	}
	bool daSamurshai::collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther) {
		apOther->someFlagByte |= 2;

		if (this->isDown == 0) { 
			damage += 5;
			if (damage >= 15) { doStateChange(&StateID_Outro); }
			else { doStateChange(&StateID_Damage); }
		}

		deathInfo.isDead = 0;
		this->flags_4FC |= (1<<(31-7));
		this->counter_504[apOther->owner->which_player] = 5;
		bouncePlayerWhenJumpedOn(apOther->owner);
		return true;
	}
	bool daSamurshai::collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther) {
		return this->collisionCat7_GroundPound(apThis, apOther);
	}
	bool daSamurshai::collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther) {
		if (this->isDown == 0) { 
			damage += 3;
			if (damage >= 15) { doStateChange(&StateID_Outro); }
			else { doStateChange(&StateID_Damage); }
		}
		return true;
	}
	bool daSamurshai::collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther){
		return true;
	}
	bool daSamurshai::collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther){
		dAcPy_vf3F8(apOther->owner, this, 3);

		deathInfo.isDead = 0;
		this->flags_4FC |= (1<<(31-7));
		this->counter_504[apOther->owner->which_player] = 0;
		return true;
	}
	bool daSamurshai::collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther){ 
		//damage += 4;
		//SpawnEffect("Wm_mr_fireball_hit", 0, &apOther->owner->pos, &apOther->owner->rot, &apOther->owner->scale);
		//PlaySoundAsync(this, SE_OBJ_FIREBALL_DISAPP);
		//if (damage >= 15) { doStateChange(&StateID_Outro); }
		return true;
	}
	bool daSamurshai::collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther) {
		//damage += 1;
		//SpawnEffect("Wm_mr_fireball_hit", 0, &apOther->owner->pos, &apOther->owner->rot, &apOther->owner->scale);
		//PlaySoundAsync(this, SE_OBJ_FIREBALL_DISAPP);		
		//if (damage >= 15) { doStateChange(&StateID_Outro); }
		return true;
	}
	bool daSamurshai::collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther) {
		return true;
	}
	bool daSamurshai::collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther) {
		//damage += 2;
		//this->spawnHitEffectAtPosition((Vec2){apOther->owner->pos.x, apOther->owner->pos.y});
		//if (damage >= 15) { doStateChange(&StateID_Outro); }
		return true;
	}


	void daSamurshai::addScoreWhenHit(void *other) { }
	void daSamurshai::powBlockActivated(bool isNotMPGP) { }

	bool daSamurshai::calculateTileCollisions() {
		// Returns true if sprite should turn, false if not.

		HandleXSpeed();
		HandleYSpeed();
		doSpriteMovement();

		cmgr_returnValue = collMgr.isOnTopOfTile();
		collMgr.calculateBelowCollisionWithSmokeEffect();

		if (isBouncing) {
			stuffRelatingToCollisions(0.1875f, 1.0f, 0.5f);
			if (speed.y != 0.0f)
				isBouncing = false;
		}

		if (collMgr.isOnTopOfTile()) {
			// Walking into a tile branch

			if (cmgr_returnValue == 0)
				isBouncing = true;

			speed.y = 0.0f;
		} 

		// Bouncing checks
		if (_34A & 4) {
			Vec v = (Vec){0.0f, 1.0f, 0.0f};
			collMgr.pSpeed = &v;

			if (collMgr.calculateAboveCollision(collMgr.outputMaybe))
				speed.y = 0.0f;

			collMgr.pSpeed = &speed;

		} else {
			if (collMgr.calculateAboveCollision(collMgr.outputMaybe))
				speed.y = 0.0f;
		}

		collMgr.calculateAdjacentCollision(0);

		// Switch Direction
		if (collMgr.outputMaybe & (0x15 << direction)) {
			if (collMgr.isOnTopOfTile()) {
				isBouncing = true;
			}
			pos.x += direction ? 2.0 : -2.0;
			return true;
		}
		return false;
	}

void daSamurshai::bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate) {
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr(name);
	this->chrAnimation.bind(&this->bodyModel, anmChr, unk);
	this->bodyModel.bindAnim(&this->chrAnimation, unk2);
	this->chrAnimation.setUpdateRate(rate);
}

int daSamurshai::onCreate() {

	isBigBoss = settings & 0xF;

	// Model Setup
		allocator.link(-1, GameHeaps[0], 0, 0x20);

		this->resFile.data = getResource("Shynja", "g3d/Shynja.brres");
		nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("Shynja");
		bodyModel.setup(mdl, &allocator, 0x224, 1, 0);

		// Animations start here
		nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr("c18_IDLE_R");
		this->chrAnimation.setup(mdl, anmChr, &this->allocator, 0);

		allocator.unlink();

	// Character Setup

		scale = (Vec){25.0, 25.0, 25.0};

		pos.y = pos.y - 16.0 * 3.0;
		rot.x = 0; // X is vertical axis
		rot.y = 0xD800; // Y is horizontal axis
		rot.z = 0; // Z is ... an axis >.>
		direction = 1; // Heading left.
		
		speed.x = 0.0;
		speed.y = 0.0;
		XSpeed = 2.5;
		max_speed.x = 50.0;
		initialPos = pos;
		topHurts = false;

	// Physics
		ActivePhysics::Info HitMeBaby;

		HitMeBaby.xDistToCenter = 0.0;
		HitMeBaby.yDistToCenter = 15.0;

		HitMeBaby.xDistToEdge = 13.0;
		HitMeBaby.yDistToEdge = 15.0;		

		HitMeBaby.category1 = 0x3;
		HitMeBaby.category2 = 0x0;
		HitMeBaby.bitfield1 = 0x4F;
		HitMeBaby.bitfield2 = 0xffbafffe;
		HitMeBaby.unkShort1C = 0;
		HitMeBaby.callback = &dEn_c::collisionCallback;

		this->aPhysics.initWithStruct(this, &HitMeBaby);
		this->aPhysics.addToList();

	// Tile collider

		// These fucking rects do something for the tile rect
		spriteSomeRectX = 48.0f;
		spriteSomeRectY = 36.0f;
		_320 = 0.0f;
		_324 = 18.0f;

		// These structs tell stupid collider what to collide with - these are from koopa troopa
		static const lineSensor_s below(-0<<12, 0<<12, 0<<12);
		static const pointSensor_s above(0<<12, 12<<12);
		static const lineSensor_s adjacent(6<<12, 9<<12, 6<<12);

		collMgr.init(this, &below, &above, &adjacent);
		collMgr.calculateBelowCollisionWithSmokeEffect();

		cmgr_returnValue = collMgr.isOnTopOfTile();

		if (collMgr.isOnTopOfTile())
			isBouncing = false;
		else
			isBouncing = true;

	// Sword Physics
		// Chuckles is left, Knuckles is Right
		ActivePhysics::Info iChuckles;
		ActivePhysics::Info iKnuckles;

		iChuckles.xDistToCenter = -27.0;
		iChuckles.yDistToCenter = 8.0;
		iChuckles.xDistToEdge   = 27.0;
		iChuckles.yDistToEdge   = 6.0;

		iKnuckles.xDistToCenter = 27.0;
		iKnuckles.yDistToCenter = 8.0;
		iKnuckles.xDistToEdge   = 27.0;
		iKnuckles.yDistToEdge   = 6.0;

		iKnuckles.category1  = iChuckles.category1  = 0x3;		iKnuckles.category2  = iChuckles.category2  = 0x0;
		iKnuckles.bitfield1  = iChuckles.bitfield1  = 0x4F;
		iKnuckles.bitfield2  = iChuckles.bitfield2  = 0x6;
		iKnuckles.unkShort1C = iChuckles.unkShort1C = 0x0;
		iKnuckles.callback   = iChuckles.callback   = ChucksAndKnucks;

		Chuckles.initWithStruct(this, &iChuckles);
		Knuckles.initWithStruct(this, &iKnuckles);

	doStateChange(&StateID_Intro);

	this->onExecute();
	return true;
}

int daSamurshai::onDelete() {
	return true;
}
int daSamurshai::onExecute() {
	acState.execute();
	updateModelMatrices();

	return true;
}
int daSamurshai::onDraw() {
	bodyModel.scheduleForDrawing();
	bodyModel._vf1C();

	return true;
}
void daSamurshai::updateModelMatrices() {
	// This won't work with wrap because I'm lazy.
	matrix.translation(pos.x, pos.y + 0.0, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	bodyModel.setDrawMatrix(matrix);
	bodyModel.setScale(&scale);
	bodyModel.calcWorld(false);
}


///////////////
// Intro State
///////////////
	void daSamurshai::beginState_Intro() { 
		this->timer = 0;

		// Stop the BGM Music
		StopBGMMusic();

		// Set the necessary Flags and make Mario enter Demo Mode
		dStage32C_c::instance->freezeMarioBossFlag = 1;
		WLClass::instance->_4 = 4;
		WLClass::instance->_8 = 0;

		MakeMarioEnterDemoMode();

		// Make sure to use the correct position
		Vec KamekPos = (Vec){pos.x - 124.0, pos.y + 104.0, 3564.0};
		S16Vec KamekRot = (S16Vec){0, 0, 0};

		rot.y = 0x2800; // Y is horizontal axis
		speed.x = 0.0;
		speed.y = 0.0;

		pos.x = pos.x - 224.0;
		pos.y = pos.y + 320.0;

		// Create And use Kameck
		Kameck = (daKameckDemo*)createChild(KAMECK_FOR_CASTLE_DEMO, (dStageActor_c*)this, 0, &KamekPos, &KamekRot, 0);
		Kameck->doStateChange(&daKameckDemo::StateID_DemoSt);
		Kameck->pos.x = Kameck->pos.x - 32.0;
	}

	void daSamurshai::executeState_Intro() { 
		this->timer += 1;

		OSReport("Timer: %d", timer);
		if (timer == 230) { bindAnimChr_and_setUpdateRate("c18_INTRO", 1, 0.0, 1.0); } 

		if ((timer > 220) && (timer < 240)) {
			pos.x += (224.0 / 20.0);
			pos.y -= (320.0 / 20.0);
		}

		if (timer == 230) { 
			SpawnEffect("Wm_en_hanapetal", 0, &pos, &(S16Vec){0,0,0}, &(Vec){1.5, 1.5, 1.5});
			SpawnEffect("Wm_ob_itemget_ring", 0, &(Vec){pos.x, pos.y + 16.0, pos.z}, &(S16Vec){0,0,0}, &(Vec){8.0, 0.1, 1.5});
			MapSoundPlayer(SoundRelatedClass, SE_OBJ_WOOD_BOX_BREAK, 1);
			MapSoundPlayer(SoundRelatedClass, SE_BOSS_KAMECK_DOWN, 1);
			this->Kameck->doStateChange(&daKameckDemo::StateID_DieFall);	
		}

		if ((timer > 230) && (timer < 350)) {
			Kameck->pos.x += (200.0 / 120.0);
			Kameck->pos.y -= (260.0 / 120.0);
		}

		int done = 0;
		if ((timer > 330) && (done == 0)) {
			u16 amt = (this->direction == 0) ? 0x2800 : 0xD800;
			done = SmoothRotation(&this->rot.y, amt, 0x2000);
		}

		if (timer == 400) { bindAnimChr_and_setUpdateRate("c18_IDLE_R", 1, 0.0, 1.0); }

		if (timer == 500) { 
			MapSoundPlayer(SoundRelatedClass, SE_BOSS_WENDY_RING_BOUND, 1);
			walkStateIsCharging = false;
			doStateChange(&StateID_Walk); 
		}
	}
	void daSamurshai::endState_Intro() { 
		CleanupKameck(this, Kameck);

		rot.y = 0xD800; // Y is horizontal axis
		direction = 1; // Heading left.
		XSpeed = 2.0;
		max_speed.x = 50.0;		
	}

///////////////
// Walk State
///////////////
	int daSamurshai::randomPlayer() {
		int players[4];
		int playerCount = 0;

		for (int i = 0; i < 4; i++) {
			if (Player_Active[i] != 0 && Player_Lives[Player_ID[i]] > 0) {
				players[playerCount] = i;
				playerCount++;
			}
		}
		return players[MakeRandomNumber(playerCount)];
	}

	void daSamurshai::beginState_Walk() {
		Chuckles.removeFromList();
		Knuckles.removeFromList();	

		bindAnimChr_and_setUpdateRate(walkStateIsCharging ? "c18_CHARGE" : "c18_RUNNING", 1, 0.0, 0.5); 
		chosenOne = GetSpecificPlayerActor(this->randomPlayer());

		speed.x = (this->direction) ? -this->XSpeed : this->XSpeed;
		if (walkStateIsCharging)
			speed.x *= 1.7f;

		this->max_speed.y = -4.0;
		this->speed.y = -4.0;
		this->y_speed_inc = -0.1875;

		OSReport("Speed: %f / %f", speed.x, max_speed.x);
	}
	void daSamurshai::executeState_Walk() { 
		MapSoundPlayer(SoundRelatedClass, SE_EMY_MOUSE_WALK, 1);

		float xDistance = pos.x - chosenOne->pos.x;
		float yDistance = pos.y - chosenOne->pos.y;

		// OSReport("Distance: %f, %f", xDistance, yDistance);
		if (!walkStateIsCharging && (xDistance >  64.0) && (direction == 0)) { doStateChange(&StateID_Turn); }
		if (!walkStateIsCharging && (xDistance < -64.0) && (direction == 1)) { doStateChange(&StateID_Turn); }

		if (xDistance < 0.0) { xDistance = -xDistance; }

		if (walkStateIsCharging) {
			amountCharged += abs(speed.x);

			// should we stop charging?
			if (amountCharged > 480.0f && !isNearWall()) {
				speed.x = (this->direction) ? -this->XSpeed : this->XSpeed;
				doStateChange(&StateID_Uppercut);
			}
		}

		// Condition for Chop
		if (!walkStateIsCharging && xDistance < 32.0) { doStateChange(&StateID_Chop); }

		// Condition For Charge Slash
		if (!walkStateIsCharging && isBigBoss) {
			if ((xDistance < 96.0) && (xDistance > 64.0)) {
				int charge = MakeRandomNumber(100);
				if (charge > 95) { doStateChange(&StateID_ChargeSlash); }
			}
		}

		// Aerial Attacks!
		if (!walkStateIsCharging && yDistance < -24.0) {
			
			// Condition For Spin Attack
			// if (xDistance < 32.0) { doStateChange(&StateID_SpinAttack); }

			// Condition for Uppercut
			if ((xDistance > 48.0) && (xDistance < 64.0) && !isNearWall())
				doStateChange(&StateID_Uppercut);
		}

		bool ret = calculateTileCollisions(); 
		if (ret) {
			doStateChange(&StateID_Turn);
		}

		if(this->chrAnimation.isAnimationDone()) {
			this->chrAnimation.setCurrentFrame(0.0);
		}
	}
	void daSamurshai::endState_Walk() { }

///////////////
// Turn State
///////////////
	void daSamurshai::beginState_Turn() {
		bindAnimChr_and_setUpdateRate("c18_RUNNING", 1, 0.0, 0.5); 
		this->speed.x = (direction) ? 0.5f : -0.5f;
		this->direction ^= 1;
		x_speed_inc = 0.0;
	}
	void daSamurshai::executeState_Turn() { 

		if(this->chrAnimation.isAnimationDone()) {
			this->chrAnimation.setCurrentFrame(0.0);
		}

		u16 amt = (this->direction == 0) ? 0x2800 : 0xD800;
		int done = SmoothRotation(&this->rot.y, amt, 0x1000);

		if(done) {
			this->doStateChange(&StateID_Walk);
		}
	}
	void daSamurshai::endState_Turn() { }

///////////////
// Chop State
///////////////
	void daSamurshai::beginState_Chop() { 
		bindAnimChr_and_setUpdateRate("c18_H_CUT_R", 1, 0.0, 1.0);
		chrAnimation.setCurrentFrame(0.0);
		timer = 0;
		
	}
	void daSamurshai::executeState_Chop() { 

		speed.x = speed.x / 1.5;

		if (chrAnimation.getCurrentFrame() == 15.0) {
			MapSoundPlayer(SoundRelatedClass, SE_EMY_CRASHER_PUNCH, 1);
			if (this->direction == 1) { 
				SpawnEffect("Wm_ob_itemget_ring", 0, &(Vec){pos.x - 18.0, pos.y + 16.0, pos.z-200.0}, &(S16Vec){0,0,0}, &(Vec){2.5, 0.5, 1.5});
				Chuckles.addToList();
			}
			else {
				SpawnEffect("Wm_ob_itemget_ring", 0, &(Vec){pos.x + 18.0, pos.y + 16.0, pos.z-200.0}, &(S16Vec){0,0,0}, &(Vec){2.5, 0.5, 1.5});
				Knuckles.addToList();
			}	
		}

		if (chrAnimation.getCurrentFrame() == 20.0) {
			MapSoundPlayer(SoundRelatedClass, SE_OBJ_WOOD_BOX_BREAK, 1);
			if (this->direction == 1) { 
				SpawnEffect("Wm_en_hit", 0, &(Vec){pos.x - 38.0, pos.y + 16.0, pos.z}, &(S16Vec){0,0,0}, &(Vec){1.0, 1.0, 1.0});
				SpawnEffect("Wm_en_hanapetal", 0, &(Vec){pos.x - 38.0, pos.y + 16.0, pos.z}, &(S16Vec){0,0,0}, &(Vec){2.0, 2.0, 2.0});
				Chuckles.removeFromList();
			}
			else {
				SpawnEffect("Wm_en_hit", 0, &(Vec){pos.x + 38.0, pos.y + 16.0, pos.z}, &(S16Vec){0,0,0}, &(Vec){1.0, 1.0, 1.0});
				SpawnEffect("Wm_en_hanapetal", 0, &(Vec){pos.x + 38.0, pos.y + 16.0, pos.z}, &(S16Vec){0,0,0}, &(Vec){2.0, 2.0, 2.0});
				Knuckles.removeFromList();	
			}
		}

		if(this->chrAnimation.isAnimationDone()) {
			timer += 1;
			if (timer > 45) {
				walkStateIsCharging = false;
				doStateChange(&StateID_Walk);
			}
		}
	}
	void daSamurshai::endState_Chop() { }

///////////////
// ChargeSlash State
///////////////
	void daSamurshai::beginState_ChargeSlash() { 
		timer = 0;
		slowDown = false;
		topHurts = false;
		speed.x = 0.0;
	}
	void daSamurshai::executeState_ChargeSlash() { 

		// End if the animation is finally over
		if ((chrAnimation.isAnimationDone()) && (slowDown) ) {
			walkStateIsCharging = false;
			doStateChange(&StateID_Walk);
			return;
		}

		// What to do if he hits a wall
		bool ret = calculateTileCollisions();
		if (ret) {
			OSReport("Hit the wall");
			bindAnimChr_and_setUpdateRate("c18_R_BLOCK_BREAK_R", 1, 0.0, 1.0);
			speed.x = (direction) ? 0.5f : -0.5f;
			topHurts = false;
			slowDown = true;
			timer = 500;
		}

		if (timer == 500) { speed.x = speed.x / 1.05; return; }

		// Begin the charge effect
		if (timer == 0) { bindAnimChr_and_setUpdateRate("c18_OB_IDLE_R", 1, 0.0, 0.5); 
			if (direction == 1)
				SpawnEffect("Wm_ob_keyget02_lighit", 0, &(Vec){pos.x + 7.0, pos.y + 14.0, pos.z - 5500.0}, &(S16Vec){0,0,0}, &(Vec){0.8, 0.8, 0.8});
			else 
				SpawnEffect("Wm_ob_keyget02_lighit", 0, &(Vec){pos.x - 7.0, pos.y + 14.0, pos.z + 5500.0}, &(S16Vec){0,0,0}, &(Vec){0.8, 0.8, 0.8});
		}

		// Start to cut
		if (timer == 60) { bindAnimChr_and_setUpdateRate("c18_H_CUT_R", 1, 0.0, 1.0); }

		// After enough charging, speed on!
		if (timer == 70) { speed.x = (direction) ? -16.0f : 16.0f; chrAnimation.setUpdateRate(0.0); }

		// He should not be able to be hurt for a while
		if (timer == 72) { topHurts = true; }

		OSReport("Speed: %f @ %d", speed.x, timer);
		float absSpeed; 
		if (speed.x < 0.0) { absSpeed = -speed.x; }
		else			   { absSpeed =  speed.x; }

		// Can be bopped again when he gets slow enough
		if (absSpeed < 1.0) { topHurts = false; chrAnimation.setUpdateRate(1.0); }

		// During the dash
		if (absSpeed > 0.0) {
			// Slow that horsey down, tiger
			if (slowDown) { speed.x = speed.x / 1.5; return; }

			effect.spawn("Wm_mr_p_iceslip", 0, &(Vec){pos.x, pos.y+8.0, pos.z}, &(S16Vec){0,0,0}, &(Vec){1.5, 1.5, 1.5});			
			SpawnEffect("Wm_en_hanapetal", 0, &pos, &(S16Vec){0,0,0}, &(Vec){1.5, 1.5, 1.5});
			SpawnEffect("Wm_ob_itemget_ring", 0, &(Vec){pos.x, pos.y + 16.0, pos.z}, &(S16Vec){0,0,0}, &(Vec){4.0, 0.1, 1.5});

			if (timer == 75) { slowDown = true; }
			// Positive if Mario is left of Samurai, negative if he is to the right
			// float xDistance = pos.x - chosenOne->pos.x;

			// // direction 1 is going left, direction 0 is going right
			// if ((xDistance > 0) && (direction == 0)) { 
			// 	SpawnEffect("Wm_ob_itemget_ring", 0, &(Vec){pos.x, pos.y + 16.0, pos.z}, &(S16Vec){0,0,0}, &(Vec){4.0, 0.1, 1.5});
			// 	slowDown = true; }

			// if ((xDistance < 0) && (direction == 1)) { 
			// 	SpawnEffect("Wm_ob_itemget_ring", 0, &(Vec){pos.x, pos.y + 16.0, pos.z}, &(S16Vec){0,0,0}, &(Vec){4.0, 0.1, 1.5});
			// 	slowDown = true; }
		}

		this->timer += 1;
	}
	void daSamurshai::endState_ChargeSlash() { 
		slowDown = false;
	}

///////////////
// Uppercut State
///////////////
	void daSamurshai::beginState_Uppercut() { 
		bindAnimChr_and_setUpdateRate("c18_H_SHOT_R", 1, 0.0, 2.0);
		slowDown = false;
		timer = 0;
	}
	void daSamurshai::executeState_Uppercut() { 

		if ((slowDown) && (this->chrAnimation.isAnimationDone())) {
			timer++;
			if (timer > 45) {
				walkStateIsCharging = false;
				doStateChange(&StateID_Walk);
			}
		}

		if (slowDown) {
			return;
		}

		if (this->chrAnimation.getCurrentFrame() == 34.0) {
			topHurts = true;
			MapSoundPlayer(SoundRelatedClass, SE_EMY_CRASHER_PUNCH, 1);
			MapSoundPlayer(SoundRelatedClass, SE_BOSS_CMN_JUMP_M, 1);

			speed.y = 4.0;
			y_speed_inc = -0.1875;

			if (this->direction == 1) { 
				effect.spawn("Wm_ob_itemget_ring", 0, &(Vec){pos.x - 48.0, pos.y + 32.0, pos.z}, &(S16Vec){0,0,0}, &(Vec){0.5, 1.5, 1.5});
				Chuckles.addToList();
			}
			else {
				effect.spawn("Wm_ob_itemget_ring", 0, &(Vec){pos.x + 48.0, pos.y + 32.0, pos.z}, &(S16Vec){0,0,0}, &(Vec){0.5, 1.5, 1.5});
				Knuckles.addToList();
			}	
		}

		if (this->chrAnimation.getCurrentFrame() == 64.0) {
			if (topHurts) {
				if (this->direction == 1) { 
					SpawnEffect("Wm_en_hit", 0, &(Vec){pos.x - 38.0, pos.y + 16.0, pos.z}, &(S16Vec){0,0,0}, &(Vec){1.0, 1.0, 1.0});
					SpawnEffect("Wm_en_hanapetal", 0, &(Vec){pos.x - 38.0, pos.y + 16.0, pos.z}, &(S16Vec){0,0,0}, &(Vec){2.0, 2.0, 2.0});
					Chuckles.removeFromList();
				}
				else {
					SpawnEffect("Wm_en_hit", 0, &(Vec){pos.x + 38.0, pos.y + 16.0, pos.z}, &(S16Vec){0,0,0}, &(Vec){1.0, 1.0, 1.0});
					SpawnEffect("Wm_en_hanapetal", 0, &(Vec){pos.x + 38.0, pos.y + 16.0, pos.z}, &(S16Vec){0,0,0}, &(Vec){2.0, 2.0, 2.0});
					Knuckles.removeFromList();	
				}

				MapSoundPlayer(SoundRelatedClass, SE_OBJ_WOOD_BOX_BREAK, 1);
				topHurts = false;
			}
		}

		if ((this->chrAnimation.getCurrentFrame() > 34.0) && (speed.y == 0)) {
			speed.x = 0.0f; 
		}

		if ((this->chrAnimation.isAnimationDone()) && (speed.y == 0.0)) { 
			bindAnimChr_and_setUpdateRate("c18_DIVING_STEAL_R", 1, 0.0, 1.0);
			slowDown = true;
		}	

		bool ret = calculateTileCollisions();
	}
	void daSamurshai::endState_Uppercut() { slowDown = false; }

///////////////
// SpinAttack State
///////////////
	void daSamurshai::beginState_SpinAttack() { 
		bindAnimChr_and_setUpdateRate("c18_NORMAL_STEAL_R", 1, 0.0, 1.0);
	}
	void daSamurshai::executeState_SpinAttack() { 

		if((int)this->chrAnimation.getCurrentFrame() == 20) {
			topHurts = true;
			PlaySoundAsync(this, SE_EMY_CRASHER_PUNCH);

			speed.y = 3.0;
			y_speed_inc = -0.1875;

			if (this->direction == 1) { Chuckles.addToList(); }
			else { Knuckles.addToList(); }	
		}

		if (((int)chrAnimation.getCurrentFrame() > 20) && ((int)this->chrAnimation.getCurrentFrame() < 60)) {
			scale = (Vec){1.0, 1.0, 1.0};
			if (this->direction == 1) { 
				effect.spawn("Wm_mr_spinjump", 0, &(Vec){pos.x - 18.0, pos.y + 16.0, pos.z}, &(S16Vec){0,0,0}, &scale);
			}
			else {
				effect.spawn("Wm_mr_spinjump", 0, &(Vec){pos.x + 18.0, pos.y + 16.0, pos.z}, &(S16Vec){0,0,0}, &scale);
			}
		}

		if((int)this->chrAnimation.getCurrentFrame() == 65) {
			topHurts = false;
			PlaySoundAsync(this, SE_EMY_CRASHER_PUNCH);

			if (this->direction == 1) { Chuckles.removeFromList(); }
			else { Knuckles.removeFromList(); }	
		}

		if ((this->chrAnimation.isAnimationDone()) && (speed.y == 0.0)) {
			walkStateIsCharging = false;
			doStateChange(&StateID_Walk);
		}

		bool ret = calculateTileCollisions();

	}
	void daSamurshai::endState_SpinAttack() { 
	}

///////////////
// Damage State
///////////////
	void daSamurshai::beginState_Damage() {
		bindAnimChr_and_setUpdateRate("c18_L_DMG_F_1_R", 1, 0.0, 1.0); 

		this->max_speed.x = 0;
		this->speed.x = 0;
		this->x_speed_inc = 0;

		this->max_speed.y = -2.0;
		this->speed.y = -2.0;
		this->y_speed_inc = -0.1875;

		this->timer = 0;
		this->isDown = 1;

		Chuckles.removeFromList();
		Knuckles.removeFromList();
		aPhysics.removeFromList();

		SpawnEffect("Wm_ob_switch", 0, &(Vec){pos.x, pos.y + 16.0, pos.z}, &(S16Vec){0,0,0}, &(Vec){1.0, 1.0, 1.0});
		MapSoundPlayer(SoundRelatedClass, SE_BOSS_CMN_DAMAGE_DEF, 1);
	}
	void daSamurshai::executeState_Damage() { 
		calculateTileCollisions();
	
		effect.spawn("Wm_en_spindamage", 0, &(Vec){pos.x, pos.y + 40.0, 0}, &(S16Vec){0,0,0}, &(Vec){1.0, 1.0, 1.0});

		if (this->chrAnimation.isAnimationDone()) {
			if 		(timer == 0) { bindAnimChr_and_setUpdateRate("c18_L_DMG_F_3_R", 1, 0.0, 1.0); timer = 1; }
			else if (timer == 1) { bindAnimChr_and_setUpdateRate("c18_L_DMG_F_4_R", 1, 0.0, 1.0); timer = 2; }
			else if (timer == 2) {
				walkStateIsCharging = true;
				amountCharged = 0.0f;
				doStateChange(&StateID_Walk);
			}
		}
	}
	void daSamurshai::endState_Damage() {
		aPhysics.addToList();

		this->isDown = 0;		
		this->rot.y = (direction) ? 0xD800 : 0x2800;
	}

///////////////
// Outro State
///////////////
	void daSamurshai::beginState_Outro() { 

		bindAnimChr_and_setUpdateRate("c18_SP_BLOCK_R", 1, 0.0, 1.0); 
		OutroSetup(this);
		aPhysics.removeFromList();

		this->max_speed.x = 0;
		this->speed.x = 0;
		this->x_speed_inc = 0;

		this->max_speed.y = -2.0;
		this->speed.y = -2.0;
		this->y_speed_inc = -0.1875;

		this->timer = 0;
		this->isDown = 1;

		Chuckles.removeFromList();
		Knuckles.removeFromList();
		MapSoundPlayer(SoundRelatedClass, SE_BOSS_CMN_DAMAGE_LAST, 1);

	}
	void daSamurshai::executeState_Outro() {
		calculateTileCollisions();

		if (this->dying == 1) { 

			if (this->timer > 180) { ExitStage(WORLD_MAP, 0, BEAT_LEVEL, MARIO_WIPE); }
			if (this->timer == 60) { PlayerVictoryCries(this); }	
			
			this->timer += 1;
			return; 
		}	

		if ((chrAnimation.getCurrentFrame() == 41.0) || (chrAnimation.getCurrentFrame() == 62.0)) {
			SpawnEffect("Wm_en_landsmoke_s", 0, &(Vec){pos.x, pos.y - 8.0, pos.z + 500.0}, &(S16Vec){0,0,0}, &(Vec){1.0, 1.0, 1.0});
		}

		if (chrAnimation.isAnimationDone()) { 
			SpawnEffect("Wm_ob_cmnshotstar", 0, &(Vec){pos.x + 8.0, pos.y - 8.0, pos.z + 500.0}, &(S16Vec){0,0,0}, &(Vec){1.75, 1.75, 1.75});
			SpawnEffect("Wm_mr_wirehit_hit", 0, &(Vec){pos.x + 8.0, pos.y, pos.z + 500.0}, &(S16Vec){0,0,0}, &(Vec){1.0, 1.0, 1.0});

			MapSoundPlayer(SoundRelatedClass, STRM_BGM_SHIRO_BOSS_CLEAR, 1);
			BossGoalForAllPlayers();

			this->dying = 1;
			this->timer = 0;	
		}
	
		this->timer += 1;
	}
	void daSamurshai::endState_Outro() { }

bool daSamurshai::isNearWall() {
	// back up our current settings
	VEC3 savePos = pos;
	VEC3 saveSpeed = speed;
	int saveDirection = direction;

	float checkLeft = (direction == 0) ? 8.0f : 104.0f;
	float checkRight = (direction == 0) ? 104.0f : 8.0f;

	bool result = false;

	speed.x = -0.1f;
	speed.y = 0.0f;
	pos.x = savePos.x - checkLeft;
	direction = 1; // left
	if (collMgr.calculateAdjacentCollision())
		result = true;

	speed.x = 0.1f;
	speed.y = 0.0f;
	pos.x = savePos.x + checkRight;
	direction = 0; // right
	if (collMgr.calculateAdjacentCollision())
		result = true;

	// restore our settings
	pos = savePos;
	speed = saveSpeed;
	direction = saveDirection;

	return result;
}


//
// processed\../src/bossPodouble.cpp
//

#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include <stage.h>
#include "boss.h"

void poodleCollisionCallback(ActivePhysics *apThis, ActivePhysics *apOther);	

void poodleCollisionCallback(ActivePhysics *apThis, ActivePhysics *apOther) {
		if (apOther->owner->name == BROS_FIREBALL) { return; }
		else if (apOther->owner->name == BROS_ICEBALL) { return; }
		else { dEn_c::collisionCallback(apThis, apOther); }
	}

class daPodouble : public daBoss {
	int onCreate();
	int onDelete();
	int onExecute();
	int onDraw();

	void updateHitboxSize();

	mHeapAllocator_c allocator;
	nw4r::g3d::ResFile resFile;

	m3d::mdl_c bodyModel;
	m3d::mdl_c fogModel;
	//m3d::mdl_c fog2Model;

	m3d::anmChr_c fleeAnimation;
	m3d::anmTexSrt_c body;

	m3d::anmChr_c fogChr;
	m3d::anmTexSrt_c fogSRT;

	//m3d::anmChr_c fog2Chr;
	//m3d::anmTexSrt_c fog2SRT;


	char isFire;
	char goingUp;
	int timer;
	float dying;
	int damage;
	float Baseline;
	char isInvulnerable;
	int countdown;

	static daPodouble *build();

	// void spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	void playerCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	void yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther);

	bool collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther);


	USING_STATES(daPodouble);
	DECLARE_STATE(Bounce);
	DECLARE_STATE(Spit);
	DECLARE_STATE(Damage);

	DECLARE_STATE(Grow);
	DECLARE_STATE(Outro);
	DECLARE_STATE(SyncDie);

};

daPodouble *daPodouble::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daPodouble));
	return new(buffer) daPodouble;
}

///////////////////////
// Externs and States
///////////////////////
	extern "C" int SmoothRotation(short* rot, u16 amt, int unk2);

	CREATE_STATE(daPodouble, Bounce);
	CREATE_STATE(daPodouble, Spit);
	CREATE_STATE(daPodouble, Damage);

	CREATE_STATE(daPodouble, Grow);
	CREATE_STATE(daPodouble, Outro);
	CREATE_STATE(daPodouble, SyncDie);


////////////////////////
// Collision Functions
////////////////////////

	void daPodouble::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther) { DamagePlayer(this, apThis, apOther); }

	void daPodouble::yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther) 					 { this->playerCollision(apThis, apOther); }
	bool daPodouble::collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther) 	 {
		this->playerCollision(apThis, apOther);
		return true;
	}
	bool daPodouble::collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther) {
		this->playerCollision(apThis, apOther);
		return true;
	}
	bool daPodouble::collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther)		 {
		this->playerCollision(apThis, apOther);
		return true;
	}

	bool daPodouble::collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther) 	{ return true; }
	bool daPodouble::collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther) { return true; }
	bool daPodouble::collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther)		{ return true; }

	bool daPodouble::collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther) {	
		apOther->owner->kill();
		return true;
	}

	bool daPodouble::collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther){
		apOther->owner->Delete(1);
		if (this->isInvulnerable) { return true; }

		if (this->isFire == 0) { 
			this->damage += 4; 

			if (this->damage < 12)  { doStateChange(&StateID_Damage); }
			else 				    { doStateChange(&StateID_Outro); }
			return true;
		} else return false;
	}

	bool daPodouble::collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther) {
		if (this->isInvulnerable) { return true; }

		if (this->isFire == 0) { 
			this->damage += 2; 

			if (this->damage < 12)  { doStateChange(&StateID_Damage); }
			else 				    { doStateChange(&StateID_Outro); }
			return true;
		} else return false;
	}

	bool daPodouble::collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther) {
		apOther->owner->Delete(1);
		if (this->isInvulnerable) { return true; }

		if (this->isFire == 1) { 
			if (apOther->owner->name == 104) { this->damage += 2; }
			else 						     { this->damage += 4; }
		
			if (this->damage < 12)  { doStateChange(&StateID_Damage); }
			else 				    { doStateChange(&StateID_Outro); }

			return true;
		}
		else { return false; }
	}


void daPodouble::updateHitboxSize() {
	aPhysics.info.xDistToEdge = 11.429f * scale.x;
	aPhysics.info.yDistToEdge = 11.429f * scale.y;
}


int daPodouble::onCreate() {

	this->isFire = this->settings >> 28;
	this->Baseline = this->pos.y - (float)((this->settings & 0xFF) * 0.8);


	allocator.link(-1, GameHeaps[0], 0, 0x20);

	// Fire or Ice
	if (this->isFire == 1) {
		this->resFile.data = getResource("bubble", "g3d/t00.brres");
	}
	else {
		this->resFile.data = getResource("bubble", "g3d/t05.brres");
	}

	// Body and anms
	nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("bubble");
	nw4r::g3d::ResAnmChr shit = resFile.GetResAnmChr(isFire ? "RedFlee" : "BlueFlee");
	bodyModel.setup(mdl, &allocator, 0x224, 1, 0);
	fleeAnimation.setup(mdl, shit, &allocator, 0);

	nw4r::g3d::ResAnmTexSrt anmRes = this->resFile.GetResAnmTexSrt("bubble");
	this->body.setup(mdl, anmRes, &this->allocator, 0, 1);
	this->body.bindEntry(&this->bodyModel, anmRes, 0, 0);
	this->bodyModel.bindAnim(&this->body, 0.0);


	// Fog up and anms
	mdl = this->resFile.GetResMdl("bubble_fog");
	fogModel.setup(mdl, &allocator, 0x124, 1, 0);

	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr("bubble_fog");
	this->fogChr.setup(mdl, anmChr, &this->allocator, 0);
	this->fogChr.bind(&this->fogModel, anmChr, 1);
	this->fogModel.bindAnim(&this->fogChr, 0.0);
	this->fogChr.setUpdateRate(1.0);

	anmRes = this->resFile.GetResAnmTexSrt("bubble_fog");
	this->fogSRT.setup(mdl, anmRes, &this->allocator, 0, 1);
	this->fogSRT.bindEntry(&this->fogModel, anmRes, 0, 0);
	this->fogModel.bindAnim(&this->fogSRT, 0.0);


	// Fog down and anms
	/*mdl = this->resFile.GetResMdl("bubble_fog2");
	fog2Model.setup(mdl, &allocator, 0x124, 1, 0);

	anmChr = this->resFile.GetResAnmChr("bubble_fog2");
	this->fog2Chr.setup(mdl, anmChr, &this->allocator, 0);
	this->fog2Chr.bind(&this->fog2Model, anmChr, 1);
	this->fogModel.bindAnim(&this->fog2Chr, 0.0);
	this->fog2Chr.setUpdateRate(1.0);

	anmRes = this->resFile.GetResAnmTexSrt("bubble_fog");
	this->fog2SRT.setup(mdl, anmRes, &this->allocator, 0, 1);
	this->fog2SRT.bindEntry(&this->fog2Model, anmRes, 0, 0);
	this->fog2Model.bindAnim(&this->fog2SRT, 0.0);*/



	allocator.unlink();


	// Stuff I do understand
	this->scale = (Vec){1.0, 1.0, 1.0};


	this->direction = (this->isFire) ? 0 : 1;
	this->countdown = (this->isFire) ? 90 : 0;

	this->rot.x = 0; // X is vertical axis
	this->rot.y = (direction) ? 0xD800 : 0x2800;
	this->rot.z = 0; // Z is ... an axis >.>
	
	this->speed.x = 0.0;
	this->speed.y = 0.0;
	this->max_speed.x = 0.0;
	this->x_speed_inc = 0.0;

	this->goingUp = 0;



	ActivePhysics::Info HitMeBaby;

	HitMeBaby.xDistToCenter = 0.0;
	HitMeBaby.yDistToCenter = 0.0;

	HitMeBaby.xDistToEdge = 40.0;
	HitMeBaby.yDistToEdge = 40.0;		

	HitMeBaby.category1 = 0x3;
	HitMeBaby.category2 = 0x0;
	HitMeBaby.bitfield1 = 0x4F;
	HitMeBaby.bitfield2 = (this->isFire) ? 0x380626 : 0x380626;
	HitMeBaby.unkShort1C = 0;
	HitMeBaby.callback = &poodleCollisionCallback;

	this->aPhysics.initWithStruct(this, &HitMeBaby);
	this->aPhysics.addToList();

	doStateChange(&StateID_Grow);

	this->onExecute();
	return true;
}

int daPodouble::onDelete() {
	return true;
}

int daPodouble::onExecute() {
	acState.execute();

	float diff = 8.57f * scale.x;
	float checkY = pos.y - diff, checkLastY = last_pos.y - diff;

	VEC2 myPos = {pos.x, checkY};
	VEC2 outBlockPos;
	float outFloat;
	s16 outAngle;
	int result = dWaterManager_c::instance->queryPosition(&myPos, &outBlockPos, &outFloat, &outAngle, currentLayerID);

	if ((!isFire && result == 0) || (isFire && result == 1)) {
		if ((checkLastY <= outFloat && outFloat < checkY) || (checkY <= outFloat && outFloat < checkLastY)) {
			VEC3 efPos = {pos.x, checkY, 6500.0f};
			VEC3 efScale = {scale.x * 0.7f, scale.y * 0.7f, scale.z * 0.7f};
			SpawnEffect(isFire ? "Wm_en_magmawave" : "Wm_en_waterwave_in", 0, &efPos, 0, &efScale);

			if (checkLastY <= outFloat && outFloat < checkY) {
				dBgGm_c::instance->makeSplash(pos.x, checkY, 0x11);
			} else {
				dBgGm_c::instance->makeSplash(pos.x, checkY, 0x10);
			}
		}
	}

	return true;
}

int daPodouble::onDraw() {
//	if (this->speed.y >= 0) {
		matrix.translation(pos.x, pos.y, pos.z);

		fogModel.setDrawMatrix(matrix);
		fogModel.setScale(&scale);
		fogModel.calcWorld(false);

		fogModel.scheduleForDrawing();
		fogModel._vf1C();
		fogChr.process();

		//if(this->fogChr.isAnimationDone())
		//	this->fogChr.setCurrentFrame(0.0);

		this->fogSRT.process();
/*	}
	else {
		matrix.translation(pos.x, pos.y, pos.z);

		fog2Model.setDrawMatrix(matrix);
		fog2Model.setScale(&scale);
		fog2Model.calcWorld(false);

		fog2Model.scheduleForDrawing();
		fog2Model._vf1C();

		if(this->fog2Chr.isAnimationDone())
			this->fog2Chr.setCurrentFrame(0.0);

		this->fog2SRT.process();
	}*/

	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	bodyModel.setDrawMatrix(matrix);
	bodyModel.setScale(&scale);
	bodyModel.calcWorld(false);

	bodyModel.scheduleForDrawing();
	bodyModel._vf1C();
	this->body.process();
	if (acState.getCurrentState() == &StateID_SyncDie)
		fleeAnimation.process();

	return true;
}


///////////////
// Grow State
///////////////
	void daPodouble::beginState_Grow() { 
		this->timer = 0;
		if (isFire) { return; }

		SetupKameck(this, Kameck);
		this->scale = (Vec){1.0, 1.0, 1.0};
	}

	void daPodouble::executeState_Grow() { 

		this->timer += 1;

		if (isFire) { 
			float scaleSpeed;

			if (timer == 150) { PlaySound(this, SE_BOSS_IGGY_WANWAN_TO_L); }
			
			if ((timer > 150) && (timer < 230)) {
				scaleSpeed = (3.5 - 1.0) / 80.0;
			
				float modifier;

				modifier = 1.0 + ((timer - 150) * scaleSpeed);
				
				this->scale = (Vec){modifier, modifier, modifier};
				this->pos.y = this->pos.y + (18/80.0);
			}

			if (timer > 420) { 
				PlaySound(this, SE_EMY_CS_MOVE_W8_BUBBLE_APP);
				doStateChange(&StateID_Bounce); 
			}

			return;
		}
		
		bool ret;
		ret = GrowBoss(this, Kameck, 1.0, 3.5, 18, this->timer);

		if (this->timer == 382) {

			int players = GetActivePlayerCount();

			Vec tempPos = (Vec){this->pos.x - 190.0, this->pos.y + 120.0, 3564.0};
			dStageActor_c *spawner = create(AC_YOSHI_EGG, 0x71010000, &tempPos, &(S16Vec){0,0,0}, 0);
			spawner->speed.x = spawner->speed.x / 2.0;
			
			if (players > 1) {
				tempPos = (Vec){this->pos.x - 190.0, this->pos.y + 120.0, 3564.0};
				spawner = create(AC_YOSHI_EGG, 0xF1010000, &tempPos, &(S16Vec){0,0,0}, 0);
				spawner->speed.x = spawner->speed.x / 2.0;
			}
			
			if (players > 2) {
				tempPos = (Vec){this->pos.x - 190.0, this->pos.y + 120.0, 3564.0};
				spawner->create(AC_YOSHI_EGG, 0x71010000, &tempPos, &(S16Vec){0,0,0}, 0);
			}

			if (players > 3) {
				tempPos = (Vec){this->pos.x - 190.0, this->pos.y + 120.0, 3564.0};
				create(AC_YOSHI_EGG, 0xF1010000, &tempPos, &(S16Vec){0,0,0}, 0);
			}
		}

		if (ret) { 	
			PlaySound(this, SE_EMY_CS_MOVE_W8_BUBBLE_APP);
			doStateChange(&StateID_Bounce); 
		}
	}
	void daPodouble::endState_Grow() { 
		this->Baseline = this->pos.y;
		if (isFire) { return; }

		CleanupKameck(this, Kameck);
	}


///////////////
// Bounce State
///////////////
	void daPodouble::beginState_Bounce() { 
		this->rot.y = (direction) ? 0xD800 : 0x2800;
		this->rot.x = 0;

		this->max_speed.y = -5.0;
		this->speed.y = -1.0;
		this->y_speed_inc = -0.1875;

		this->goingUp = 0;
	}
	void daPodouble::executeState_Bounce() { 

		if (this->countdown) { 
			this->countdown--; 
			return; }

		HandleYSpeed();
		doSpriteMovement();

		if (this->pos.y < this->Baseline) { 
			this->speed.y = 7.5; 		
			this->goingUp = 1; }

		if (-0.1 < this->speed.y < 0.1) { 
			if (this->goingUp == 1) { doStateChange(&StateID_Spit); } }


		// Check for stupid liquid junk
		checkLiquidImmersion(&(Vec2){this->pos.x, this->pos.y}, 3.0f);

	}
	void daPodouble::endState_Bounce() { 
		this->speed.y = 0.0;
		this->y_speed_inc = 0.0;
	}


///////////////
// Spit State
///////////////
	void daPodouble::beginState_Spit() { this->timer = 0; }
	void daPodouble::executeState_Spit() { 

		if (this->timer < 0x40) {
			this->rot.x -= 0x80;
		}

		else if (this->timer < 0x48) {
			this->rot.x += 0x400;
		}

		else if (this->timer == 0x48) {
			if (this->isFire == 0) {
				dStageActor_c *spawner = create(BROS_ICEBALL, 0x10, &this->pos, &(S16Vec){0,0,0}, 0);
				*((u32 *) (((char *) spawner) + 0x3DC)) = this->id;

				int num;
				num = GenerateRandomNumber(20) - 10;
				float modifier = (float)(num) / 10.0;

				spawner->max_speed.y += modifier;
				spawner->speed.y += modifier;

				num = GenerateRandomNumber(20) - 10;
				modifier = (float)(num) / 10.0;

				spawner->max_speed.x += modifier;
				spawner->speed.x += modifier;


				PlaySoundAsync(this, SE_EMY_ICE_BROS_ICE);
				doStateChange(&StateID_Bounce);
			}
			else {
				dStageActor_c *spawner = create(BROS_FIREBALL, 0, &this->pos, &(S16Vec){0,0,0}, 0);

				int num;
				num = GenerateRandomNumber(20);
				float modifier = (float)(num) / 5.0;

				spawner->max_speed.y += modifier;
				spawner->speed.y += modifier;

				num = GenerateRandomNumber(20);
				modifier = (float)(num) / 10.0;

				spawner->max_speed.x += modifier;
				spawner->speed.x += modifier;

				PlaySoundAsync(this, SE_EMY_FIRE_BROS_FIRE);
				doStateChange(&StateID_Bounce);
			}
		}

		this->timer += 1;
	}
	void daPodouble::endState_Spit() { 
		this->speed.y = -1.0;
		this->y_speed_inc = -0.1875;
	}


///////////////
// Damage State
///////////////
	void daPodouble::beginState_Damage() { 
		this->timer = 0;
		this->isInvulnerable = 1;

		if (this->isFire == 0) {
			PlaySoundAsync(this, SE_OBJ_PNGN_ICE_THAW);
			SpawnEffect("Wm_ob_iceevaporate", 0, &this->pos, &(S16Vec){0,0,0}, &(Vec){2.0, 2.0, 2.0}); }
		else {
			PlaySoundAsync(this, SE_EMY_FIRE_SNAKE_EXTINCT);
			SpawnEffect("Wm_en_firesnk_icehitsmk_b", 0, &this->pos, &(S16Vec){0,0,0}, &(Vec){3.0, 3.0, 3.0}); }
	}
	void daPodouble::executeState_Damage() { 

		int amt;
		amt = sin(this->timer * 3.14 / 4.0) * 0x2000;

		this->rot.y = amt;
		this->rot.y += (direction) ? 0xD800 : 0x2800;

		float targetScale = 3.5f - (damage * 0.175f);
		if (scale.x > targetScale) {
			float newScale = scale.x - 0.006f;
			if (newScale < targetScale)
				newScale = targetScale;
			scale.x = scale.y = scale.z = newScale;
			updateHitboxSize();
		}

		if (this->timer > 180) {
			doStateChange(&StateID_Bounce);
		}
		
		if ((this->timer == 60) || (this->timer == 80) || (this->timer == 100)) {
			if (this->isFire == 0) {
				dStageActor_c *spawner = create(BROS_ICEBALL, 0x10, &this->pos, &(S16Vec){0,0,0}, 0);
				*((u32 *) (((char *) spawner) + 0x3DC)) = this->id;
				PlaySoundAsync(this, SE_EMY_ICE_BROS_ICE);
			}
			else {
				create(BROS_FIREBALL, 0, &this->pos, &(S16Vec){0,0,0}, 0);
				PlaySoundAsync(this, SE_EMY_FIRE_BROS_FIRE);
			}
		}

		this->timer += 1;

	}
	void daPodouble::endState_Damage() { 
		this->isInvulnerable = 0;
	}


///////////////
// Outro State
///////////////
	void daPodouble::beginState_Outro() { 

		daPodouble *other = (daPodouble*)FindActorByType(SHIP_WINDOW, 0);
			if (other->id == this->id) {
				other = (daPodouble*)FindActorByType(SHIP_WINDOW, (Actor*)this);
			}
		other->doStateChange(&StateID_SyncDie);

		for (int i = 0; i < 4; i++) {
			dAcPy_c *player = (dAcPy_c*)GetSpecificPlayerActor(i);
			if (player && player->getYoshi()) {
				player->input.setFlag(dPlayerInput_c::NO_SHAKING);
			}
		}

		OutroSetup(this);
		this->timer = 0;

	}
	void daPodouble::executeState_Outro() {

		if (this->dying == 1) { 
			if (this->timer > 180) { ExitStage(WORLD_MAP, 0, BEAT_LEVEL, MARIO_WIPE); }
			if (this->timer == 60) { PlayerVictoryCries(this); }	
			
			this->timer += 1;
			return; 
		}	

		bool ret;
		ret = ShrinkBoss(this, &this->pos, 3.5, this->timer);

		if (ret == true) { 
			BossExplode(this, &this->pos); 
			this->dying = 1;
			this->timer = 0;	
			nw4r::snd::SoundHandle handle;
			PlaySoundWithFunctionB4(SoundRelatedClass, &handle, SE_EMY_BUBBLE_EXTINCT, 1);
		}
	
		this->timer += 1;

	}
	void daPodouble::endState_Outro() { }



///////////////
// SyncDie State
///////////////
	void daPodouble::beginState_SyncDie() { 

		this->removeMyActivePhysics();
		this->timer = 0;

		nw4r::g3d::ResAnmChr anmChr = resFile.GetResAnmChr(isFire ? "RedFlee" : "BlueFlee");

		fleeAnimation.bind(&bodyModel, anmChr, 1);
		bodyModel.bindAnim(&fleeAnimation, 0.0);
		fleeAnimation.setUpdateRate(0.5f);

		fogChr.bind(&fogModel, anmChr, 1);
		fogModel.bindAnim(&fogChr, 0.0f);
		fogChr.setUpdateRate(0.5f);

		PlaySound(this, SE_EMY_GABON_ROCK_THROW);
	}
	void daPodouble::executeState_SyncDie() {
		if (fleeAnimation.isAnimationDone())
			Delete(0);
	}
	void daPodouble::endState_SyncDie() { }

//
// processed\../src/bossTopman.cpp
//

#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include <stage.h>
#include "boss.h"

class daDreidel : public daBoss {
public:
	int onCreate();
	int onDelete();
	int onExecute();
	int onDraw();

	mHeapAllocator_c allocator;
	nw4r::g3d::ResFile resFile;

	m3d::mdl_c bodyModel;

	m3d::anmChr_c chrAnimation;

	char isDown;
	float XSpeed;
	u32 cmgr_returnValue;
	bool isBouncing;
	char isInSpace;
	char fromBehind;
	char isInvulnerable;
	int isInvulnerableCountdown;
	int isTurningCountdown;
	char charging;
	int flashing;

	float dying;

	static daDreidel *build();

	void bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate);
	void updateModelMatrices();
	bool calculateTileCollisions();

	void powBlockActivated(bool isNotMPGP);

	void spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	void playerCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	void yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther);

	bool collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther);

	void addScoreWhenHit(void *other);

	USING_STATES(daDreidel);
	DECLARE_STATE(Walk);
	DECLARE_STATE(Turn);
	DECLARE_STATE(KnockBack);

	DECLARE_STATE(ChargePrep);
	DECLARE_STATE(Charge);
	DECLARE_STATE(ChargeRecover);
	DECLARE_STATE(Damage);

	DECLARE_STATE(Grow);
	DECLARE_STATE(Outro);

};

daDreidel *daDreidel::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daDreidel));
	return new(buffer) daDreidel;
}

///////////////////////
// Externs and States
///////////////////////
	extern "C" void *EN_LandbarrelPlayerCollision(dEn_c* t, ActivePhysics *apThis, ActivePhysics *apOther);
	// CalculateDistanceFromActorsNextPosToFurthestPlayer???
	extern "C" int SmoothRotation(short* rot, u16 amt, int unk2);


	CREATE_STATE(daDreidel, Walk);
	CREATE_STATE(daDreidel, Turn);
	CREATE_STATE(daDreidel, KnockBack);

	CREATE_STATE(daDreidel, ChargePrep);
	CREATE_STATE(daDreidel, Charge);
	CREATE_STATE(daDreidel, ChargeRecover);
	CREATE_STATE(daDreidel, Damage);

	CREATE_STATE(daDreidel, Grow);
	CREATE_STATE(daDreidel, Outro);

	// 	begoman_attack2"	// wobble back and forth tilted forwards
	// 	begoman_attack3"	// Leaned forward, antennae extended
	// 	begoman_damage"		// Bounces back slightly
	// 	begoman_damage2"	// Stops spinning and wobbles to the ground like a top
	// 	begoman_stand"		// Stands still, waiting
	// 	begoman_wait"		// Dizzily Wobbles
	// 	begoman_wait2"		// spins around just slightly
	// 	begoman_attack"		// Rocks backwards, and then attacks to an upright position, pulsing out his antennae


////////////////////////
// Collision Functions
////////////////////////
	// void topCollisionCallback(ActivePhysics *apThis, ActivePhysics *apOther);

	// void topCollisionCallback(ActivePhysics *apThis, ActivePhysics *apOther) {
	// 	OSReport("Collided with %d", apOther->owner->name);
	// 	if (apOther->owner->name != 041) { 
	// 		dEn_c::collisionCallback(apThis, apOther); 
	// 	}
	// 	else {
	// 		daDreidel *actor = (daDreidel*)apThis->owner;
	// 		actor->damage += 1;
	// 		actor->doStateChange(&daDreidel::StateID_Damage);

	// 		if (actor->damage > 2) { actor->doStateChange(&daDreidel::StateID_Outro); }
	// 	}
	// }
	void daDreidel::addScoreWhenHit(void *other) { };

	void daDreidel::spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
		if (apOther->owner->name == 390) { //time to get hurt
			OSReport("YO SUP I'M A TOPMAN AND I'M COLLIDING WITH A FUCKING WALL [%d]\n", damage);
			if (this->isInvulnerable) {
				OSReport("I'm invulnerable so I'm ignoring it\n");
				return;
			}
			this->damage += 1;
			OSReport("I'm increasing my damage to %d\n", damage);
			doStateChange(&StateID_Damage);

			if (this->damage == 3) { doStateChange(&StateID_Outro); }
		}
		else { dEn_c::spriteCollision(apThis, apOther); }
	}

	void daDreidel::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther) {


		char hitType;
		hitType = usedForDeterminingStatePress_or_playerCollision(this, apThis, apOther, 0);

		if(hitType > 0) {
			this->_vf220(apOther->owner);
			this->counter_504[apOther->owner->which_player] = 0;
		} else {

			if (this->charging == 1) {

				char MarioPos;

				if (this->pos.x < apOther->owner->pos.x) { MarioPos = 0; } // Mario is to the right
				else 									 { MarioPos = 1; } // Mario is to the left
				

				if (this->direction != MarioPos) {	// Mario is stnading behind the boss
					EN_LandbarrelPlayerCollision(this, apThis, apOther);
					if (MarioPos == 1)  { this->direction = 1; }
					else 				{ this->direction = 0; }

					doStateChange(&StateID_KnockBack);
				}

				else { // Mario is standing in front of the boss
					this->_vf220(apOther->owner);
					EN_LandbarrelPlayerCollision(this, apThis, apOther);
				}
			}

			else if (this->isInvulnerable == 0) { 

				if (this->pos.x > apOther->owner->pos.x) {
					this->direction = 1;
				}
				else {
					this->direction = 0;
				}
			
				this->counter_504[apOther->owner->which_player] = 0;
				EN_LandbarrelPlayerCollision(this, apThis, apOther);
				doStateChange(&StateID_KnockBack);
			}
			else {
				this->counter_504[apOther->owner->which_player] = 0;
				EN_LandbarrelPlayerCollision(this, apThis, apOther);
			}
		} 

		deathInfo.isDead = 0;
		this->flags_4FC |= (1<<(31-7));
	}

	void daDreidel::yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
		this->playerCollision(apThis, apOther);
	}

	bool daDreidel::collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther) {
		DamagePlayer(this, apThis, apOther);
		return true;
	}
	bool daDreidel::collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther) {
		DamagePlayer(this, apThis, apOther);
		return true;
	}
	bool daDreidel::collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther) {
		DamagePlayer(this, apThis, apOther);
		return true;
	}

	bool daDreidel::collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther) { 
		if (this->isInvulnerable == 0) { 
			doStateChange(&StateID_KnockBack);
		}
		return true;
	}
	bool daDreidel::collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther){ 
		if (this->isInvulnerable == 0) { 
			doStateChange(&StateID_KnockBack);
		}
		return true;
	}


	bool daDreidel::collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther){ return true; }
	bool daDreidel::collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther) { return true; }
	bool daDreidel::collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther){ return true; }
	bool daDreidel::collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther) { return true; }
	bool daDreidel::collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther) { return true; }

void daDreidel::powBlockActivated(bool isNotMPGP) { }


bool daDreidel::calculateTileCollisions() {
	// Returns true if sprite should turn, false if not.

	HandleXSpeed();
	HandleYSpeed();
	doSpriteMovement();

	cmgr_returnValue = collMgr.isOnTopOfTile();
	collMgr.calculateBelowCollisionWithSmokeEffect();

	if (isBouncing) {
		stuffRelatingToCollisions(0.1875f, 1.0f, 0.5f);
		if (speed.y != 0.0f)
			isBouncing = false;
	}

	float xDelta = pos.x - last_pos.x;
	if (xDelta >= 0.0f)
		direction = 0;
	else
		direction = 1;

	if (collMgr.isOnTopOfTile()) {
		// Walking into a tile branch

		if (cmgr_returnValue == 0)
			isBouncing = true;

		if (speed.x != 0.0f) {
			//playWmEnIronEffect();
		}

		speed.y = 0.0f;

		// u32 blah = collMgr.s_80070760();
		// u8 one = (blah & 0xFF);
		// static const float incs[5] = {0.00390625f, 0.0078125f, 0.015625f, 0.0234375f, 0.03125f};
		// x_speed_inc = incs[one];
		max_speed.x = (direction == 1) ? -1.0f : 1.0f;
	} else {
		x_speed_inc = 0.0f;
	}

	// Bouncing checks
	if (_34A & 4) {
		Vec v = (Vec){0.0f, 1.0f, 0.0f};
		collMgr.pSpeed = &v;

		if (collMgr.calculateAboveCollision(collMgr.outputMaybe))
			speed.y = 0.0f;

		collMgr.pSpeed = &speed;

	} else {
		if (collMgr.calculateAboveCollision(collMgr.outputMaybe))
			speed.y = 0.0f;
	}

	collMgr.calculateAdjacentCollision(0);

	// Switch Direction
	if (collMgr.outputMaybe & (0x15 << direction)) {
		if (collMgr.isOnTopOfTile()) {
			isBouncing = true;
		}
		return true;
	}
	return false;
}

void daDreidel::bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate) {
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr(name);
	this->chrAnimation.bind(&this->bodyModel, anmChr, unk);
	this->bodyModel.bindAnim(&this->chrAnimation, unk2);
	this->chrAnimation.setUpdateRate(rate);
}

int daDreidel::onCreate() {
	
	// Model creation	
	allocator.link(-1, GameHeaps[0], 0, 0x20);

	this->resFile.data = getResource("topman", "g3d/begoman_spike.brres");
	nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("begoman");
	bodyModel.setup(mdl, &allocator, 0x224, 1, 0);
	SetupTextures_Map(&bodyModel, 0);


	// Animations start here
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr("begoman_wait");
	this->chrAnimation.setup(mdl, anmChr, &this->allocator, 0);

	allocator.unlink();

	// Stuff I do understand
	this->scale = (Vec){0.2, 0.2, 0.2};

	// this->pos.y = this->pos.y + 30.0; // X is vertical axis
	this->rot.x = 0; // X is vertical axis
	this->rot.y = 0xD800; // Y is horizontal axis
	this->rot.z = 0; // Z is ... an axis >.>
	this->direction = 1; // Heading left.
	
	this->speed.x = 0.0;
	this->speed.y = 0.0;
	this->max_speed.x = 1.1;
	this->x_speed_inc = 0.0;
	this->XSpeed = 1.1;

	this->isInSpace = this->settings & 0xF;
	this->fromBehind = 0;
	this->flashing = 0; 


	ActivePhysics::Info HitMeBaby;

	HitMeBaby.xDistToCenter = 0.0;
	HitMeBaby.yDistToCenter = 24.0;

	HitMeBaby.xDistToEdge = 28.0;
	HitMeBaby.yDistToEdge = 24.0;		

	HitMeBaby.category1 = 0x3;
	HitMeBaby.category2 = 0x0;
	HitMeBaby.bitfield1 = 0x4F;
	HitMeBaby.bitfield2 = 0xffbafffe;
	HitMeBaby.unkShort1C = 0;
	HitMeBaby.callback = &dEn_c::collisionCallback;

	this->aPhysics.initWithStruct(this, &HitMeBaby);
	this->aPhysics.addToList();


	// Tile collider

	// These fucking rects do something for the tile rect
	spriteSomeRectX = 28.0f;
	spriteSomeRectY = 32.0f;
	_320 = 0.0f;
	_324 = 16.0f;

	// These structs tell stupid collider what to collide with - these are from koopa troopa
	static const lineSensor_s below(12<<12, 4<<12, 0<<12);
	static const pointSensor_s above(0<<12, 12<<12);
	static const lineSensor_s adjacent(6<<12, 9<<12, 6<<12);

	collMgr.init(this, &below, &above, &adjacent);
	collMgr.calculateBelowCollisionWithSmokeEffect();

	cmgr_returnValue = collMgr.isOnTopOfTile();

	if (collMgr.isOnTopOfTile())
		isBouncing = false;
	else
		isBouncing = true;


	// State Changers
	bindAnimChr_and_setUpdateRate("begoman_wait2", 1, 0.0, 1.0); 
	doStateChange(&StateID_Grow);

	this->onExecute();
	return true;
}

int daDreidel::onDelete() {
	return true;
}

int daDreidel::onExecute() {
	acState.execute();
	updateModelMatrices();
	
	return true;
}

int daDreidel::onDraw() {

	if (this->isInvulnerable == 1) {
		this->flashing++;
	}

	if (this->flashing < 5) {
		bodyModel.scheduleForDrawing();
	}

	bodyModel._vf1C();

	if (this->flashing > 8) { this->flashing = 0; }

	return true;
}

void daDreidel::updateModelMatrices() {
	matrix.translation(pos.x, pos.y - 2.0, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	bodyModel.setDrawMatrix(matrix);
	bodyModel.setScale(&scale);
	bodyModel.calcWorld(false);
}



///////////////
// Grow State
///////////////
	void daDreidel::beginState_Grow() { 
		bindAnimChr_and_setUpdateRate("begoman_wait2", 1, 0.0, 0.75); 

		this->timer = 0;

		SetupKameck(this, Kameck);
	}

	void daDreidel::executeState_Grow() { 
		
		if(this->chrAnimation.isAnimationDone()) {
			this->chrAnimation.setCurrentFrame(0.0);
		}

		this->timer += 1;
		
		bool ret;
		ret = GrowBoss(this, Kameck, 0.2, 0.4, 0, this->timer);

		if (ret) { 	
			PlaySound(this, SE_EMY_MECHAKOOPA_BOUND);
			doStateChange(&StateID_Walk); 
		}
	}
	void daDreidel::endState_Grow() { 
		this->chrAnimation.setUpdateRate(1.0);
		CleanupKameck(this, Kameck);
	}


///////////////
// Walk State
///////////////
	void daDreidel::beginState_Walk() {
		this->max_speed.x = (this->direction) ? -this->XSpeed : this->XSpeed;
		this->speed.x = (direction) ? -1.2f : 1.2f;

		this->max_speed.y = (this->isInSpace) ? -2.0 : -4.0;
		this->speed.y = 	(this->isInSpace) ? -2.0 : -4.0;
		this->y_speed_inc = (this->isInSpace) ? -0.09375 : -0.1875;

		this->isTurningCountdown = 0;
	}
	void daDreidel::executeState_Walk() { 

		if (this->isInvulnerableCountdown > 0) {
			this->isInvulnerableCountdown--;
		}
		else {
			this->isInvulnerable = 0;
			this->flashing = 0;
		}

		PlaySound(this, SE_BOSS_JR_CROWN_JR_RIDE); // 5
	
		bool ret = calculateTileCollisions();
		if (ret) {
			doStateChange(&StateID_Turn);
		}

		u8 dir = dSprite_c__getXDirectionOfFurthestPlayerRelativeToVEC3(this, this->pos);
		if (dir != this->direction) {
			this->isTurningCountdown++;
		}

		if (this->isTurningCountdown > 60) { doStateChange(&StateID_Turn); }

		if(this->chrAnimation.isAnimationDone()) {
			this->chrAnimation.setCurrentFrame(0.0);
		}
	}
	void daDreidel::endState_Walk() { this->timer += 1; }


///////////////
// Turn State
///////////////
	void daDreidel::beginState_Turn() {
		this->direction ^= 1;
		this->speed.x = 0.0;
	}
	void daDreidel::executeState_Turn() { 

		if(this->chrAnimation.isAnimationDone()) {
			this->chrAnimation.setCurrentFrame(0.0);
		}

		u16 amt = (this->direction == 0) ? 0x2800 : 0xD800;
		int done = SmoothRotation(&this->rot.y, amt, 0x800);

		if(done) {
			if (this->damage > 0) 	{ doStateChange(&StateID_ChargePrep); }
			else 					{ doStateChange(&StateID_Walk); }
		}
	}
	void daDreidel::endState_Turn() { this->rot.y = (this->direction) ? 0xD800 : 0x2800; }


///////////////
// Knockback State
///////////////
	void daDreidel::beginState_KnockBack() {
		bindAnimChr_and_setUpdateRate("begoman_damage", 1, 0.0, 0.65); 

		this->max_speed.x = (this->direction) ? 6.5f : -6.5f;
		this->speed.x = (this->direction) ? 6.5f : -6.5f;
	}
	void daDreidel::executeState_KnockBack() { 

		bool ret = calculateTileCollisions();
		if (ret) {
			this->max_speed.x = -this->max_speed.x;
			this->speed.x = -this->speed.x;
		}
		this->speed.x = this->speed.x / 1.08;

		bodyModel._vf1C();
		if(this->chrAnimation.isAnimationDone()) {
			if (this->damage > 0) 	{ doStateChange(&StateID_ChargePrep); }
			else 					{ doStateChange(&StateID_Walk); }
		}

	}
	void daDreidel::endState_KnockBack() { 
		if (this->rot.y == 0x2800) {
			// CreateEffect(&this->pos, &(S16Vec){0,0,0}, &(Vec){1.0, 1.0, 1.0}, 175);
			this->direction = 0; 
		}
		else {
			// CreateEffect(&this->pos, &(S16Vec){0,0,0}, &(Vec){1.0, 1.0, 1.0}, 192);
			this->direction = 1; 
		}
		// this->direction ^= 1;
		bindAnimChr_and_setUpdateRate("begoman_wait2", 1, 0.0, 1.0); 
	}


///////////////
// ChargePrep State
///////////////
	void daDreidel::beginState_ChargePrep() {
		bindAnimChr_and_setUpdateRate("begoman_attack", 1, 0.0, 0.9); 
	}
	void daDreidel::executeState_ChargePrep() { 
		if(this->chrAnimation.isAnimationDone()) {
			doStateChange(&StateID_Charge);
		}
	}
	void daDreidel::endState_ChargePrep() { }


///////////////
// Charge State
///////////////
	void daDreidel::beginState_Charge() {
		bindAnimChr_and_setUpdateRate("begoman_attack3", 1, 0.0, 1.0); 
		this->timer = 0;
		this->isTurningCountdown = 0;

		this->max_speed.x = (this->direction) ? -this->XSpeed : this->XSpeed;
		this->speed.x = (direction) ? -2.6f : 2.6f;

		this->max_speed.y = (this->isInSpace) ? -2.0 : -4.0;
		this->speed.y = 	(this->isInSpace) ? -2.0 : -4.0;
		this->y_speed_inc = (this->isInSpace) ? -0.09375 : -0.1875;

		this->charging = 1;
	}
	void daDreidel::executeState_Charge() { 

		if (this->isInvulnerableCountdown > 0) {
			this->isInvulnerableCountdown--;
		}
		else {
			this->isInvulnerable = 0;
			this->flashing = 0;
		}

		if(this->chrAnimation.isAnimationDone()) {
			this->chrAnimation.setCurrentFrame(0.0);
		}

		bool ret = calculateTileCollisions();
		if (ret) {
			doStateChange(&StateID_ChargeRecover);
		}

		u8 dir = dSprite_c__getXDirectionOfFurthestPlayerRelativeToVEC3(this, this->pos);
		if (dir != this->direction) {
			this->isTurningCountdown++;
			this->speed.x = this->speed.x / 1.04;
		}

		if (this->isTurningCountdown > 90) { doStateChange(&StateID_Turn); }
	}
	void daDreidel::endState_Charge() { 
		this->charging = 0; 

		this->counter_504[0] = 0;
		this->counter_504[1] = 0;
		this->counter_504[2] = 0;
		this->counter_504[3] = 0;
	}


///////////////
// ChargeRecover State
///////////////
	void daDreidel::beginState_ChargeRecover() {
		bindAnimChr_and_setUpdateRate("begoman_stand", 1, 0.0, 0.5); 
	}
	void daDreidel::executeState_ChargeRecover() { 
		if(this->chrAnimation.isAnimationDone()) {
			doStateChange(&StateID_Turn);
		}
	}
	void daDreidel::endState_ChargeRecover() { }


///////////////
// Damage State
///////////////
	void daDreidel::beginState_Damage() {
		this->isInvulnerable = 1;
		bindAnimChr_and_setUpdateRate("begoman_wait", 1, 0.0, 0.75); 
		this->timer = 0;

		PlaySound(this, SE_PLY_TOUCH_BIRI);
		PlaySound(this, SE_BOSS_JR_ELEC_SPARK);
		PlaySound(this, SE_EMY_BIRIKYU_SPARK);

		Vec front = {this->pos.x-4.0, this->pos.y+8.0, 5500.0};
		Vec back = {this->pos.x-4.0, this->pos.y+8.0, -5500.0};

		SpawnEffect("Wm_ob_cmnspark", 0, &front, 0, &(Vec){2.5, 2.5, 1.5});
		SpawnEffect("Wm_mr_wirehit_line", 0, &front, 0, &(Vec){1.5, 1.5, 1.5});
		SpawnEffect("Wm_mr_kick_grain", 0, &front, 0, &(Vec){1.5, 1.5, 1.5});
		SpawnEffect("Wm_mr_wirehit_glow", 0, &back, 0, &(Vec){2.5, 2.5, 2.5});
	}
	void daDreidel::executeState_Damage() { 

		if(this->chrAnimation.isAnimationDone()) {
			this->timer += 1;
			if (this->timer == 1) {

				u8 dir = dSprite_c__getXDirectionOfFurthestPlayerRelativeToVEC3(this, this->pos);
				if (dir != this->direction) {
					doStateChange(&StateID_Turn);
				}
				else {
					if (this->damage > 1) 	{ doStateChange(&StateID_ChargePrep); }
					else 					{ doStateChange(&StateID_Walk); }
				}
			}
			else {
			}
			this->chrAnimation.setCurrentFrame(0.0);
		}
	}
	void daDreidel::endState_Damage() { 
		bindAnimChr_and_setUpdateRate("begoman_wait2", 1, 0.0, 1.0); 
		this->isInvulnerableCountdown = 90; 
	}


///////////////
// Outro State
///////////////
	void daDreidel::beginState_Outro() { 
		OutroSetup(this);
	}
	void daDreidel::executeState_Outro() {

		if (this->dying == 1) { 
			if (this->timer > 180) { ExitStage(WORLD_MAP, 0, BEAT_LEVEL, MARIO_WIPE); }
			if (this->timer == 60) { PlayerVictoryCries(this); }	
			
			this->timer += 1;
			return; 
		}	

		bool ret;
		ret = ShrinkBoss(this, &this->pos, 0.5, this->timer);

		if (ret == true) 	{ 
			BossExplode(this, &this->pos); 
			this->dying = 1;
			this->timer = 0;	
		}
		else 		{ PlaySound(this, SE_EMY_BUBBLE_EXTINCT); }
	
		this->timer += 1;
	}
	void daDreidel::endState_Outro() { }

//
// processed\../src/bossPlayerClown.cpp
//

#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include "boss.h"

// #define cField(TYPE, OFFSET) (*(TYPE*)(((u32)clown) + (OFFSET)))
// #define cPlayerOccupying cField(dStageActor_c*, 0x738)
// #define cModel cField(m3d::mdl_c*, 0xFEC)
// #define cTimer cField(u32, 0xFEC+sizeof(m3d::mdl_c))
// #define cAllocator cField(mHeapAllocator_c*, 0xFD0)

#define cPlayerOccupying (*(dStageActor_c**)(((u32)(clown)) + 0x738 ))
#define cAllocator ((mHeapAllocator_c*)(((u32)(clown)) + 0xFD0 ))
#define cModel ((m3d::mdl_c*)( ((u32)(clown)) + 0xFEC ))
#define cTimer (*(u32*)((u32)(clown) + sizeof(m3d::mdl_c) + 0xFEC ))

extern "C" int PClownCarExecute(dEn_c *clown);
extern "C" void PClownCarAfterCreate(dEn_c *clown, u32);
extern "C" int PClownCarDraw(dEn_c *clown);
extern "C" void PClownCarMove(dEn_c *clown);


const char* PCCarcNameList [] = {
	"koopaJr_clown_ply",
	NULL
};



int CConDraw(dEn_c *clown) {
	// setup cannon model
	clown->matrix.translation(clown->pos.x, clown->pos.y + 8.0, clown->pos.z-100.0);
	short newrotz = -0x2000;
	short newroty = ((clown->rot.y * 0x4000) / 0x800) - 0x4000;
	short newrotx;
	if (clown->rot.x < 0x8000) {
		newrotx = -clown->rot.x;
	}
	else {
		newrotx = clown->rot.x;
	}
	// OSReport("Angle?: %x, %x", clown->rot.y, newroty);
	clown->matrix.applyRotationYXZ(&clown->rot.x, &newroty, &newrotz);

	cModel->setDrawMatrix(clown->matrix);
	cModel->setScale(&(Vec){0.25, 0.5, 0.25});
	cModel->calcWorld(false);

	cModel->scheduleForDrawing();


	return PClownCarDraw(clown);
	// run normal clown function
}
extern dStateBase_c JrClownEndDemoState;
extern dStateBase_c JrClownDemoWaitState;
extern dStateBase_c ClownDemoWaitState;

int CConExecute(dEn_c *clown) {
	// A REALLY TERRIBLE HACK.
	float saveX = clown->pos.x;
	float saveY = clown->pos.y;

	int ret = PClownCarExecute(clown);

	dStateBase_c *state = clown->acState.getCurrentState();
	if (state == &JrClownEndDemoState || state == &JrClownDemoWaitState || state == &ClownDemoWaitState) {
		clown->pos.x = saveX;
		clown->pos.y = saveY;
		clown->speed.x = 0.0f;
		clown->speed.y = 0.0f;
	}
}

void CCafterCreate(dEn_c *clown, u32 param) {

	clown->scale.x *= 1.25;
	clown->scale.y *= 1.25;
	clown->scale.z *= 1.25;

	// setup the model
	nw4r::g3d::ResFile resFile;

	cAllocator->link(-1, GameHeaps[0], 0, 0x20);

	resFile.data = getResource("koopaJr_clown_ply", "g3d/cannon.brres");
	nw4r::g3d::ResMdl mdl = resFile.GetResMdl("Cannon");
	cModel->setup(mdl, cAllocator, 0x224, 1, 0);
	SetupTextures_MapObj(cModel, 0);

	cAllocator->unlink();

	// Original AfterCreate
	PClownCarAfterCreate(clown, param);

	int playerCount = 0;
	for (int i = 0; i < 4; i++)
		if (Player_Active[i])
			playerCount++;

	if ((clown->settings & 0xF) != 0) {
		int playerID = (clown->settings & 0xF) - 1;
		if (playerID >= playerCount)
			clown->Delete(1);
	}
}

void CConExecuteMove(dEn_c *clown) {

	u8 player = cPlayerOccupying->which_player;
	// OSReport("Angle = %x, %x, %x", (GetSpecificPlayerActor(player))->rot.y, (GetSpecificPlayerActor(player))->rot.x, (GetSpecificPlayerActor(player))->rot.z);
	// OSReport("Clown = %x, %x, %x", (clown)->rot.y, (clown)->rot.x, (clown)->rot.z);

	Vec tempPos;
	
	u32 buttonPushed = Remocon_GetPressed(GetRemoconMng()->controllers[cPlayerOccupying->which_player]);
	if (buttonPushed & 0x0100) {

		if (cTimer > 90) {
			if (clown->direction == 0) { // Going right
				tempPos = (Vec){clown->pos.x + 32.0, clown->pos.y + 32.0, 3564.0};
				dStageActor_c *spawned = CreateActor(657, 0, tempPos, 0, 0);
				spawned->speed.x = 5.0;
			}
			else {
				tempPos = (Vec){clown->pos.x - 32.0, clown->pos.y + 32.0, 3564.0};
				dStageActor_c *spawned = CreateActor(657, 0, tempPos, 0, 0);
				spawned->speed.x = -5.0;
			}

			SpawnEffect("Wm_en_killervanish", 0, &tempPos, &(S16Vec){0,0,0}, &(Vec){0.1, 0.1, 0.1});
			nw4r::snd::SoundHandle handle;
			PlaySoundWithFunctionB4(SoundRelatedClass, &handle, SE_OBJ_HOUDAI_S_SHOT, 1);

		cTimer = 0;
		}
	}

	cTimer++;

	ClassWithCameraInfo *cwci = ClassWithCameraInfo::instance;
	float leftBound = cwci->screenLeft + 12.0f;
	float rightBound = (cwci->screenLeft + cwci->screenWidth) - 12.0f;
	if (clown->pos.x < leftBound)
		clown->pos.x = leftBound;
	if (clown->pos.x > rightBound)
		clown->pos.x = rightBound;

	// run normal move
	PClownCarMove(clown);
}


extern "C" m3d::mdl_c *__ct__Q23m3d5mdl_cFv(m3d::mdl_c *mdl);
extern "C" mHeapAllocator_c *__ct__16mHeapAllocator_cFv(mHeapAllocator_c *al);
extern "C" dEn_c *__ct__20daJrClownForPlayer_cFv(dEn_c *clown);

dEn_c *newClownCtor(dEn_c *clown) {
	__ct__20daJrClownForPlayer_cFv(clown);
	__ct__16mHeapAllocator_cFv(cAllocator);
	__ct__Q23m3d5mdl_cFv(cModel);
	return clown;
}


extern "C" void __dt__Q23m3d5mdl_cFv(m3d::mdl_c *mdl, u32 willDelete);
extern "C" void __dt__16mHeapAllocator_cFv(mHeapAllocator_c *al, u32 willDelete);
extern "C" void __dt__20daJrClownForPlayer_cFv(dEn_c *clown, u32 willDelete);

extern "C" u32 sAllocatorFunc__FrmHeap;

void newClownDtor(dEn_c *clown, u32 willDelete) {
	void **al = (void **)(((u32)clown) + 0x524);
	if (*al != &sAllocatorFunc__FrmHeap) {
		OSReport("oh no! bad allocator %p\n", *al);
		*al = &sAllocatorFunc__FrmHeap;
	}

	__dt__Q23m3d5mdl_cFv(cModel, 0xFFFFFFFF);
	__dt__16mHeapAllocator_cFv(cAllocator, 0xFFFFFFFF);
	__dt__20daJrClownForPlayer_cFv(clown, willDelete);
}

extern "C" void JrClownForPlayer_playAccelSound() {
	nw4r::snd::SoundHandle handle;
	PlaySoundWithFunctionB4(SoundRelatedClass, &handle, SE_PLY_CROWN_ACC, 1);
}







// Below is the projectile used byt eh clown car - Replaces WM_PAKKUN

class daClownShot : public dEn_c {
	int onCreate();
	int onExecute();
	int onDraw();

	mHeapAllocator_c allocator;
	nw4r::g3d::ResFile resFile;
	m3d::mdl_c bodyModel;

	mEf::es2 effect;
	static daClownShot *build();

	void playerCollision(ActivePhysics *apThis, ActivePhysics *apOther);
};

void daClownShot::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther) { }

daClownShot *daClownShot::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daClownShot));
	return new(buffer) daClownShot;
}


int daClownShot::onCreate() {
	allocator.link(-1, GameHeaps[0], 0, 0x20);
	this->resFile.data = getResource("koopaJr_clown_ply", "g3d/houdai_ball.brres");
	nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("houdai_ball");
	bodyModel.setup(mdl, &allocator, 0x224, 1, 0);
	allocator.unlink();

	ActivePhysics::Info GreatBalls;
	
	GreatBalls.xDistToCenter = 0.0;
	GreatBalls.yDistToCenter = 0.0;
	GreatBalls.xDistToEdge = 8.0;
	GreatBalls.yDistToEdge = 7.0;
	
	GreatBalls.category1 = 0x3;
	GreatBalls.category2 = 0x0;
	GreatBalls.bitfield1 = 0x6F;
	GreatBalls.bitfield2 = 0xffbafffe;
	GreatBalls.unkShort1C = 0;
	GreatBalls.callback = &dEn_c::collisionCallback;

	this->aPhysics.initWithStruct(this, &GreatBalls);
	this->aPhysics.addToList();


	// These fucking rects do something for the tile rect
	spriteSomeRectX = 8.0f;
	spriteSomeRectY = 8.0f;
	_320 = 0.0f;
	_324 = 0.0f;

	u32 flags = SENSOR_BREAK_BRICK | SENSOR_BREAK_BLOCK | SENSOR_80000000;
	static const lineSensor_s below(flags, 12<<12, 4<<12, 0<<12);
	static const pointSensor_s above(flags, 0<<12, 12<<12);
	static const lineSensor_s adjacent(flags, 6<<12, 9<<12, 6<<12);

	collMgr.init(this, &below, &above, &adjacent);
	collMgr.calculateBelowCollisionWithSmokeEffect();


	this->speed.y = 4.0;
	this->y_speed_inc = -0.1875;
	
	this->onExecute();
	return true;
}

int daClownShot::onDraw() {
	matrix.translation(this->pos.x, this->pos.y, this->pos.z);

	matrix.applyRotationYXZ(&this->rot.x, &this->rot.y, &this->rot.z);

	bodyModel.setDrawMatrix(matrix);
	bodyModel.setScale(&scale);
	bodyModel.calcWorld(true);

	bodyModel.scheduleForDrawing();
	return true;
}


int daClownShot::onExecute() {
	HandleXSpeed();
	HandleYSpeed();
	doSpriteMovement();

	collMgr.calculateBelowCollisionWithSmokeEffect();
	collMgr.calculateAboveCollision(0);
	collMgr.calculateAdjacentCollision();
	if (collMgr.outputMaybe) {
		SpawnEffect("Wm_en_burst_m", 0, &pos, &(S16Vec){0,0,0}, &(Vec){1.0, 1.0, 1.0});
		nw4r::snd::SoundHandle handle;
		PlaySoundWithFunctionB4(SoundRelatedClass, &handle, SE_OBJ_TARU_BREAK, 1);
		Delete(1);
		return true;
	}

	effect.spawn("Wm_en_killersmoke", 0, &(Vec){pos.x, pos.y, pos.z}, &(S16Vec){0,0,0}, &(Vec){0.7, 0.7, 0.7});

	float rect[] = {0.0, 0.0, 8.0, 8.0};
	int ret = this->outOfZone(this->pos, (float*)&rect, this->currentZoneID);
	if(ret) {
		this->Delete(1);
	}

	return true;
}




// This is for making clown shots able to kill other shit

extern "C" bool Amp_NewPreSpriteCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
	// apThis = amp, apOther = other thing
	dEn_c *amp = (dEn_c*)apThis->owner;

	if (apOther->info.category2 == 9) {
		if (amp->collisionCat9_RollingObject(apThis, apOther))
			return true;
	} else if (apOther->owner->name == WM_PAKKUN) {
		amp->killByDieFall(apOther->owner);
		return true;
	}

	return false;
}

extern "C" void KazanRock_Explode(void *kazanRock);
extern "C" void KazanRock_OriginalCollisionCallback(ActivePhysics *apThis, ActivePhysics *apOther);
extern "C" void KazanRock_CollisionCallback(ActivePhysics *apThis, ActivePhysics *apOther) {
	if (apOther->owner->name == WM_PAKKUN) {
		apThis->someFlagByte |= 2;
		KazanRock_Explode(apThis->owner);
	} else {
		KazanRock_OriginalCollisionCallback(apThis, apOther);
	}
}


//
// processed\../src/bossCaptainBowser.cpp
//

#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include <stage.h>
#include "boss.h"

extern "C" void *StageScreen;

extern u32 GameTimer;

#define time_macro *(u32*)((GameTimer) + 0x4)


// const char* effects_name_list [] = {
// 	"Wm_jr_electricline", // cool
// 	"Wm_jr_fireattack", // cool
// 	"Wm_jr_firehit", // cool
// 	"Wm_jr_fireplace", // cool
// 	"Wm_jr_fireplace_ind", // cool
// 	"Wm_jr_shot",
// 	"Wm_jr_sweat",
// 	"Wm_ko_fireattack", // cool
// 	"Wm_ko_firehit", // cool
// 	"Wm_ko_firehitdie01", // cool
// 	"Wm_ko_firehitdie02", // cool
// 	"Wm_ko_firehitdie03", // cool
// 	"Wm_ko_magmapocha",
// 	"Wm_ko_magmapochabig",
// 	"Wm_ko_shout", // cool
// 	"Wm_ko_shout02", // cool
// 	"Wm_seacloudout", // cool
// };


const char* CBarcNameList [] = {
	"KoopaShip",
	"koopa",
	"choropoo",
	"koopa_clown_bomb",
	"dossun",
	NULL	
};


class daCaptainBowser : public daBoss {
public:
	int onCreate();
	int onDelete();
	int onExecute();
	int onDraw();
	int afterExecute(int param);

	mHeapAllocator_c allocator;
	nw4r::g3d::ResFile resFile;
	nw4r::g3d::ResFile shipFile;

	m3d::mdl_c bodyModel;
	m3d::mdl_c shipModel;

	m3d::anmChr_c chrAnimation;
	m3d::anmChr_c shipAnm;

	nw4r::snd::SoundHandle prplSound;

	static const int SHIP_SCOLL_COUNT = 7;
	Physics shipSColls[SHIP_SCOLL_COUNT];
	mEf::es2 effect;

	mEf::es2 shipDmgA;
	mEf::es2 shipDmgB;
	mEf::es2 shipDmgC;
	mEf::es2 shipDmgD;
	mEf::es2 shipDmgE;

	mEf::es2 flamethrowerEffect;
	mEf::es2 flamethrowerEffectInd;

	nw4r::g3d::ResFile flamethrowerRF;
	m3d::mdl_c flamethrowerModel;
	m3d::anmTexSrt_c flamethrowerAnim;
	bool renderFlamethrowerModel;
	ActivePhysics flameCollision;

#define flameOffsetX -160.0f
#define flameOffsetY 150.0f
#define flameZ 1900.0f
	Vec flameScale;

	char isAngry;
	char isInvulnerable;
	char isIntro;
	int maxDamage;
	int playerCount;
	float roarLen;

	float bowserX, bowserY;
	s16 bowserRotX, bowserRotY;
	float bowserXSpeed, bowserYSpeed, bowserMaxYSpeed, bowserYAccel;

	s16 shipRotY;
	float sinTimerX;
	float sinTimerY;
	bool sinTimerXRunning, sinTimerYRunning, stopMoving;

	bool shipAnmFinished;
	float explosionBottomBound;

	bool deathSequenceRunning;
	bool forceClownEnds;
	int timerWhenCrashHappens;
	int timerForVictoryDance;
	void initiateDeathSequence();

	int clownCount;
	dEn_c *clownPointers[4];
	VEC2 clownDests[4];

	int saveTimer;
	bool exitedFlag;

	static daCaptainBowser *build();

	void bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate);

	void spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	void playerCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther);

	void addScoreWhenHit(void *other);

	USING_STATES(daCaptainBowser);
	DECLARE_STATE(Wait);
	DECLARE_STATE(Throw);
	DECLARE_STATE(Fire);

	DECLARE_STATE(Roar);
	DECLARE_STATE(Damage);

	DECLARE_STATE(Intro);
	DECLARE_STATE(FinalAttack);
	DECLARE_STATE(Outro);
	DECLARE_STATE(PanToExit);

};

daCaptainBowser *daCaptainBowser::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daCaptainBowser));
	return new(buffer) daCaptainBowser;
}

///////////////////////
// Externs and States
///////////////////////


	CREATE_STATE(daCaptainBowser, Wait);
	CREATE_STATE(daCaptainBowser, Throw);
	CREATE_STATE(daCaptainBowser, Fire);

	CREATE_STATE(daCaptainBowser, Roar);
	CREATE_STATE(daCaptainBowser, Damage);

	CREATE_STATE(daCaptainBowser, Intro);
	CREATE_STATE(daCaptainBowser, FinalAttack);
	CREATE_STATE(daCaptainBowser, Outro);
	CREATE_STATE(daCaptainBowser, PanToExit);



////////////////////////
// Collision Functions
////////////////////////
void daCaptainBowser::addScoreWhenHit(void *other) { };

bool daCaptainBowser::collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther) {
	apOther->owner->kill();
	SpawnEffect("Wm_en_burst_m", 0, &apOther->owner->pos, &(S16Vec){0,0,0}, &(Vec){1.0, 1.0, 1.0});
	apThis->someFlagByte |= 2;
	return true;
}

void daCaptainBowser::spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
	if (apOther->owner->name == WM_PAKKUN) { //time to get hurt
		if (this->isInvulnerable) {
			return;
		}
		this->damage -= 1;

		spawnHitEffectAtPosition((Vec2){apOther->owner->pos.x, apOther->owner->pos.y});		

		SpawnEffect("Wm_en_burst_m", 0, &apOther->owner->pos, &(S16Vec){0,0,0}, &(Vec){1.0, 1.0, 1.0});
		PlaySound(apOther->owner, SE_BOSS_CMN_STOMPED);
		apOther->owner->Delete(1);

		if (this->damage == this->maxDamage/2) 	{ doStateChange(&StateID_Roar); }
		else if (this->damage < 0)  			{ initiateDeathSequence(); }
		else 									{ doStateChange(&StateID_Damage); }
	}
}

void daCaptainBowser::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
	DamagePlayer(this, apThis, apOther);
}



void daCaptainBowser::bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate) {
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr(name);
	this->chrAnimation.bind(&this->bodyModel, anmChr, unk);
	this->bodyModel.bindAnim(&this->chrAnimation, unk2);
	this->chrAnimation.setUpdateRate(rate);
}

int daCaptainBowser::onCreate() {
	bowserX = -148.0f;
	bowserY = 122.0f;
	bowserRotY = 0xD800;

	shipRotY = 0x4000;
	sinTimerXRunning = true;
	sinTimerYRunning = true;
	
	// Model creation	
	allocator.link(-1, GameHeaps[0], 0, 0x20);

	// B-b-b-bad boy Bowsaa
	this->resFile.data = getResource("koopa", "g3d/koopa.brres");
	nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("koopa");
	bodyModel.setup(mdl, &allocator, 0x224, 1, 0);
	SetupTextures_Boss(&bodyModel, 0);

	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr("kp_wait");
	this->chrAnimation.setup(mdl, anmChr, &this->allocator, 0);


	// A ship off the ol' block
	this->shipFile.data = getResource("KoopaShip", "g3d/KoopaShip.brres");
	nw4r::g3d::ResMdl mdlShip = this->shipFile.GetResMdl("KoopaShip");
	shipModel.setup(mdlShip, &allocator, 0x224, 1, 0);
	SetupTextures_MapObj(&shipModel, 0);

	nw4r::g3d::ResAnmChr anmChrShip = this->shipFile.GetResAnmChr("KoopaShip");
	this->shipAnm.setup(mdlShip, anmChrShip, &this->allocator, 0);
	this->shipAnm.bind(&this->shipModel, anmChrShip, 1);
	this->shipModel.bindAnim(&this->shipAnm, 0.0);
	this->shipAnm.setUpdateRate(1.0);

	// Flamethrower
	flamethrowerRF.data = getResource("fire_cannon", "g3d/flame_koopa.brres");

	nw4r::g3d::ResMdl ftResMdl = flamethrowerRF.GetResMdl("fire_effect1x6_right");
	nw4r::g3d::ResAnmTexSrt ftResAnm = flamethrowerRF.GetResAnmTexSrt("fire_effect1x6_right");

	flamethrowerModel.setup(ftResMdl, &allocator, 0x224, 1, 0);
	SetupTextures_MapObj(&flamethrowerModel, 0);

	flamethrowerAnim.setup(ftResMdl, ftResAnm, &allocator, 0, 1);
	flamethrowerAnim.bindEntry(&flamethrowerModel, ftResAnm, 0, 0);
	flamethrowerModel.bindAnim(&flamethrowerAnim, 0.0f);
	flamethrowerAnim.setFrameForEntry(0.0f, 0);
	flamethrowerAnim.setUpdateRate(1.0f);

	allocator.unlink();

	// Prep the goods
	this->playerCount = GetActivePlayerCount();
	this->maxDamage = 24;

	pos.z = 8000.0;
	this->scale = (Vec){0.57, 0.57, 0.57};

	this->damage = this->maxDamage;
	static const float scX1[] = {-176.447600, 64.340078, -157.925471, -157.925471, -158.561530, 49.742932, 48.957043};
	static const float scY1[] = {128.217300, 86.427956, 54.136191, -36.090801, 22.765115, 162.568398, 66.849169};
	static const float scX2[] = {-96.022566, 150.126781, 173.523155, 81.887358, -50.343171, 84.419332, 117.889270};
	static const float scY2[] = {54.136191, 54.136191, -36.090801, -79.779661, -26.166291, 142.846802, -2.083058};
	static s16 sRots[] = {-8192, -8192, -8192};

	for (int i = 0; i < SHIP_SCOLL_COUNT; i++) {
		shipSColls[i].setup(this,
				scX1[i], scY1[i], scX2[i], scY2[i],
				0, 0, 0, 1, 0);
		if (i >= 4)
			shipSColls[i].setPtrToRotation(&sRots[i - 4]);
	}

	// Bowser Physics!
	ActivePhysics::Info BowserPhysics;
	BowserPhysics.xDistToCenter = -152.0;
	BowserPhysics.yDistToCenter = 152.0;

	BowserPhysics.xDistToEdge = 28.0;
	BowserPhysics.yDistToEdge = 26.0;

	BowserPhysics.category1 = 0x3;
	BowserPhysics.category2 = 0x0;
	BowserPhysics.bitfield1 = 0x4F;
	BowserPhysics.bitfield2 = 0x8028E;
	BowserPhysics.unkShort1C = 0;
	BowserPhysics.callback = &dEn_c::collisionCallback;

	this->aPhysics.initWithStruct(this, &BowserPhysics);
	


	// State Changers
	this->isIntro = 3;
	doStateChange(&StateID_Intro);

	return true;
}

int daCaptainBowser::onDelete() {
	if (prplSound.Exists())
		prplSound.Stop(0);
	return true;
}

int daCaptainBowser::afterExecute(int param) {
	return dEn_c::afterExecute(param);
}

int daCaptainBowser::onExecute() {
	if (forceClownEnds) {
		dEn_c *clownIter = 0;
		while (clownIter = (dEn_c*)dEn_c::search(JR_CLOWN_FOR_PLAYER, clownIter)) {
			clownIter->counter_500 = 120; // what a terrible hack.
			float *pPropRotationIncrement = (float*)(((u32)clownIter) + 0x740);
			*pPropRotationIncrement = 30.0f;

			SmoothRotation((s16*)(((u32)clownIter) + 0xD4A), 0, 0x300);
		}
	}

	acState.execute();
	if (saveTimer > 0)
		time_macro = saveTimer;

	if (!prplSound.Exists() && acState.getCurrentState() != &StateID_Outro && acState.getCurrentState() != &StateID_PanToExit) {
		PlaySoundWithFunctionB4(SoundRelatedClass, &prplSound, SE_BOSS_SHIP_PRPL, 1);
	}

	aPhysics.info.xDistToCenter = bowserX;
	aPhysics.info.yDistToCenter = bowserY + 28.0f;

	bodyModel._vf1C();
	shipModel._vf1C();

	for (int i = 0; i < SHIP_SCOLL_COUNT; i++)
		shipSColls[i].update();

	float xmod = sinTimerXRunning ? (sin(this->sinTimerX * 3.14 / 180.0) * 60.0) : 0.0f;
	float ymod = sin(this->sinTimerY * 3.14 / 130.0) * (sinTimerYRunning ? 84.0 : 10.0);

	if(this->isIntro == 0) {
		pos.x = ClassWithCameraInfo::instance->screenCentreX + 200.0 + xmod;
		pos.y = ClassWithCameraInfo::instance->screenCentreY - 180.0 + ymod;

		sinTimerX++;
		if (sinTimerX >= 360) {
			sinTimerX = 0;
			if (stopMoving)
				sinTimerXRunning = false;
		}

		sinTimerY++;
		if (sinTimerY >= 260) {
			sinTimerY = 0;
			if (stopMoving)
				sinTimerYRunning = false;
		}
	}

	if(this->shipAnm.isAnimationDone() && acState.getCurrentState() != &StateID_Outro && acState.getCurrentState() != &StateID_PanToExit) {
		this->shipAnm.setCurrentFrame(0.0);
	}
	
	return true;
}

int daCaptainBowser::onDraw() {

	if(this->isIntro > 2) { return false; }

	matrix.translation(pos.x+bowserX, pos.y+bowserY, pos.z-200.0); // 136.0 is the bottom of the platform footing
	matrix.applyRotationYXZ(&bowserRotX, &bowserRotY, &rot.z);

	bodyModel.setDrawMatrix(matrix);
	bodyModel.setScale(&(Vec){1.0, 1.0, 1.0});
	bodyModel.calcWorld(false);

	bodyModel.scheduleForDrawing();


	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &shipRotY, &rot.z);

	shipModel.setDrawMatrix(matrix);
	shipModel.setScale(&scale);
	shipModel.calcWorld(false);

	shipModel.scheduleForDrawing();

	if (renderFlamethrowerModel) {
		matrix.translation(pos.x + flameOffsetX, pos.y + flameOffsetY, flameZ);
		s16 thing = 0x8000;
		matrix.applyRotationZ(&thing);

		flamethrowerModel.setDrawMatrix(matrix);
		flamethrowerModel.setScale(&flameScale);
		flamethrowerModel.calcWorld(false);

		flamethrowerModel.scheduleForDrawing();
	}

	return true;
}


//////////////////
// State Intro
//////////////////
	void daCaptainBowser::beginState_Intro() {
		this->timer = 0;
		bindAnimChr_and_setUpdateRate("kp_wait", 1, 0.0, 1.5); 
		this->isInvulnerable = 1;
		roarLen = 300;
	}
	void daCaptainBowser::executeState_Intro() { 

		if (this->chrAnimation.isAnimationDone()) { 
			// End the intro
			if (this->isIntro == 1) {
				OSReport("We're done: %d", this->timer);
				doStateChange(&StateID_Wait); 
				return;
			}
			this->chrAnimation.setCurrentFrame(0.0); }

		// Screen Rumble
		if ((this->timer > 180) && (this->timer < 420)) {
		// if (this->timer == 180) {
			// Do Rumbly - 807CD3AC

			pos.x = ClassWithCameraInfo::instance->screenCentreX;
			pos.y = ClassWithCameraInfo::instance->screenCentreY;

			ShakeScreen(StageScreen, 2, 1, 0, 0);
			PlaySoundAsync(this, SE_BOSS_KOOPA_RUMBLE1); // 0x5D4
			// Stage80::instance->ShakeScreen(self, 5, 1, 0);
		}

		// Bowser Flies In
		if (this->timer == 422) {
			this->isIntro = 2;

			for (int i = 0; i < SHIP_SCOLL_COUNT; i++)
				shipSColls[i].addToList();
		}

		if (this->timer > 420) {
			int effectiveTimer = min(timer, 719U);
			pos.x = ClassWithCameraInfo::instance->screenCentreX + ((effectiveTimer - 420.0) * 1.5) - ((roarLen * 1.5) - 200.0);
			pos.y = ClassWithCameraInfo::instance->screenCentreY - 380.0 + ((effectiveTimer - 420.0) * 1.5) - ((roarLen * 1.5) - 200.0);
		}

		// Bowser does a shitty roar
		if (this->timer == (roarLen - 190 + 420)) { 
			this->isIntro = 1; 
			bindAnimChr_and_setUpdateRate("kp_roar3", 1, 0.0, 1.0); 
		}
		if (this->timer > (roarLen - 190 + 420)) {

			if (this->chrAnimation.getCurrentFrame() == 53.0) {
				PlaySound(this, SE_VOC_KP_L_SHOUT);
			}

			if (this->chrAnimation.getCurrentFrame() > 53.0) { 
				ShakeScreen(StageScreen, 2, 2, 0, 0);
				effect.spawn("Wm_ko_shout", 0, &(Vec){pos.x-182.0, pos.y+132.0, pos.z}, &(S16Vec){0,0,0x7000}, &(Vec){1.0, 1.0, 1.0});
			}
		}

		this->timer++;
	}
	void daCaptainBowser::endState_Intro() { 

		this->aPhysics.addToList();	

		this->isInvulnerable = 0;
		this->isIntro = 0;
	}


//////////////////
// State Wait
//////////////////
	void daCaptainBowser::beginState_Wait() {
		if (this->isAngry == 0) {
			bindAnimChr_and_setUpdateRate("kp_wait", 1, 0.0, 1.5); 
		}
		else {
			bindAnimChr_and_setUpdateRate("kp_wait", 1, 0.0, 2.0); 
		}
	}
	void daCaptainBowser::executeState_Wait() { 

	if (this->chrAnimation.isAnimationDone()) { 
		this->chrAnimation.setCurrentFrame(0.0); 

		int num = GenerateRandomNumber(4);

		if (num == 0) {
			doStateChange(&StateID_Fire); 		
		}
		else{
			doStateChange(&StateID_Throw); 		
		}
	}

	}
	void daCaptainBowser::endState_Wait() {  }


//////////////////
// State Throw
//////////////////
	void daCaptainBowser::beginState_Throw() {
		bindAnimChr_and_setUpdateRate("break", 1, 0.0, 1.0); 
		this->timer = 0;
	}
	void daCaptainBowser::executeState_Throw() { 

		if (this->chrAnimation.getCurrentFrame() == 60.0) { // throw back
			int num = GenerateRandomNumber(4);
			CreateActor(0x29F, 0x101 + ((num + 1) * 0x10), (Vec){pos.x+bowserX, pos.y+bowserY, pos.z}, 0, 0);
		}

		if (this->chrAnimation.getCurrentFrame() == 126.0) { // throw front
			int num = GenerateRandomNumber(4);
			CreateActor(0x29F, ((num + 1) * 0x10) + 1, (Vec){pos.x+bowserX, pos.y+bowserY, pos.z}, 0, 0);
		}

		if (this->chrAnimation.isAnimationDone()) { 
			this->chrAnimation.setCurrentFrame(0.0);
			if (this->isAngry == 1) {
				if (this->timer == 1) {
					doStateChange(&StateID_Wait); 
				}
			}	
			else {
				doStateChange(&StateID_Wait); 
			}		

			this->timer++;
		}

	}
	void daCaptainBowser::endState_Throw() {  }


//////////////////
// State Fire
//////////////////
	void daCaptainBowser::beginState_Fire() {
		bindAnimChr_and_setUpdateRate("fire1", 1, 0.0, 1.5); 
		this->timer = 0;
	}
	void daCaptainBowser::executeState_Fire() { 

		if (this->chrAnimation.getCurrentFrame() == 70.5) { // spit fire
			PlaySound(this, SE_BOSS_KOOPA_L_FIRE_SHOT);
			CreateActor(WM_ANTLION, 0, (Vec){pos.x-172.0, pos.y+152.0, pos.z}, 0, 0);
		}

		if (this->chrAnimation.isAnimationDone()) { 
			this->chrAnimation.setCurrentFrame(0.0);
			if (this->isAngry == 1) {
				if (this->timer == 1) {
					doStateChange(&StateID_Wait); 
				}
			}	
			else {
				doStateChange(&StateID_Wait); 
			}		

			this->timer++;
		}

	}
	void daCaptainBowser::endState_Fire() {  }



//////////////////
// State Roar
//////////////////
	void daCaptainBowser::beginState_Roar() {
		bindAnimChr_and_setUpdateRate("kp_roar3", 1, 0.0, 1.0); 
		this->isInvulnerable = 1;
		this->timer = 0;
	}
	void daCaptainBowser::executeState_Roar() { 

		if (this->chrAnimation.getCurrentFrame() == 53.0) { // This is where the smackdown starts
			nw4r::snd::SoundHandle handle;
			PlaySoundWithFunctionB4(SoundRelatedClass, &handle, SE_VOC_KP_L_SHOUT, 1);
		}

		if (this->chrAnimation.getCurrentFrame() > 53.0) { // This is where the smackdown starts
			effect.spawn("Wm_ko_shout", 0, &(Vec){pos.x-174.0, pos.y+140.0, pos.z}, &(S16Vec){0,0,0x7000}, &(Vec){1.0, 1.0, 1.0});
		}

		if (this->chrAnimation.isAnimationDone()) { 
			doStateChange(deathSequenceRunning ? &StateID_FinalAttack : &StateID_Wait); 
		}

	}
	void daCaptainBowser::endState_Roar() { 
		this->isInvulnerable = 0;
		this->isAngry = 1;
	}



//////////////////
// State Damage
//////////////////
	void daCaptainBowser::beginState_Damage() {
		bindAnimChr_and_setUpdateRate("grow_big", 1, 0.0, 1.0); 
		this->isInvulnerable = 1;
		this->chrAnimation.setCurrentFrame(9.0); 

		PlaySound(this, SE_VOC_KP_DAMAGE_HPDP);
	}
	void daCaptainBowser::executeState_Damage() { 

		if (this->chrAnimation.getCurrentFrame() == 65.0) { // stop it here before it's too late
			doStateChange(&StateID_Wait); 
		}

	}
	void daCaptainBowser::endState_Damage() { this->isInvulnerable = 0; }


//////////////////
// State Outro
//////////////////
void daCaptainBowser::initiateDeathSequence() {
	deathSequenceRunning = true;
	stopMoving = true;
	saveTimer = time_macro;

	StopBGMMusic();
	dFlagMgr_c::instance->set(31, 0, false, false, false);

	*((u32*)(((char*)dBgGm_c::instance) + 0x900F0)) = 999;

	doStateChange(&StateID_Roar);
}

void daCaptainBowser::beginState_FinalAttack() {
	isInvulnerable = true;

	bindAnimChr_and_setUpdateRate("mastfail", 1, 0.0, 1.0f); 

	timer = 0;
	flameScale.x = flameScale.y = flameScale.z = 0.0f;
}

void daCaptainBowser::executeState_FinalAttack() {
	timer++;

	if (timer == 56) {
		nw4r::snd::SoundHandle handle;
		PlaySoundWithFunctionB4(SoundRelatedClass, &handle, SE_VOC_JR_CHARGE, 1);
		handle.SetPitch(0.25f);
	} else if (timer == 185) {
		nw4r::snd::SoundHandle handle;
		PlaySoundWithFunctionB4(SoundRelatedClass, &handle, SE_VOC_KP_SHOUT, 1);
	} else if (timer == 348) {
		nw4r::snd::SoundHandle handle;
		PlaySoundWithFunctionB4(SoundRelatedClass, &handle, SE_EMY_BIG_PAKKUN_DAMAGE_1, 1);
	}

	if (timer > 204 && timer < 315) {
		if (timer == 205) {
			// First thing
			renderFlamethrowerModel = true;

			static const ActivePhysics::Info fcInfo = {
				0.0f, 0.0f, 0.0f, 0.0f, // placeholder
				3, 0, 0x4F, 0x8028E, 0,
				&dEn_c::collisionCallback
			};
			flameCollision.initWithStruct(this, &fcInfo);
			flameCollision.addToList();
		}
		if (timer < 289) {
			if (flameScale.y < 2.0f) {
				flameScale.y = flameScale.y = flameScale.z = flameScale.y + 0.1f;
			} else {
				flameScale.y = flameScale.y = flameScale.z = 2.5f;
			}
		}
		flameScale.x = flameScale.y * 1.5f;

		flameCollision.info.xDistToCenter = flameOffsetX - (48.0f * flameScale.x);
		flameCollision.info.yDistToCenter = flameOffsetY;
		flameCollision.info.xDistToEdge = 48.0f * flameScale.x;
		flameCollision.info.yDistToEdge = 7.0f * flameScale.y;

		Vec efPos = {pos.x + flameOffsetX, pos.y + flameOffsetY, flameZ};
		S16Vec efRot = {-0x4000, 0, 0};
		//flamethrowerEffect.spawn("Wm_en_fireburner", 0, &efPos, &efRot, &flameScale);
		flamethrowerEffectInd.spawn("Wm_en_fireburner6ind", 0, &efPos, &efRot, &flameScale);
		flamethrowerModel._vf1C();
		flamethrowerAnim.process();
	} else if (timer == 315) {
		renderFlamethrowerModel = false;
		flameCollision.removeFromList();
		aPhysics.removeFromList();

		Vec efPos = {pos.x + flameOffsetX - 6.0f, pos.y + flameOffsetY + 1.0f, pos.z};
		float offsets[] = {0.0f, 3.0f, 0.0f, -3.0f};
		for (int i = 0; i < 20; i++) {
			efPos.y = pos.y + flameOffsetY + offsets[i & 3];
			SpawnEffect("Wm_mr_fireball_hit01", 0, &efPos, &(S16Vec){0,0,0}, &(Vec){1.5f, 1.5f, 1.5f});
			efPos.x -= 12.0f;
		}

		nw4r::snd::SoundHandle handle;
		PlaySoundWithFunctionB4(SoundRelatedClass, &handle, SE_OBJ_PAIPO, 1);
	} else if (timer == 348) {
		nw4r::snd::SoundHandle handle;
		PlaySoundWithFunctionB4(SoundRelatedClass, &handle, SE_DEMO_ED_BALLOON_LAND, 1);

		Vec efPos = {pos.x - 72.0f, pos.y + 170.0f, pos.z};
		SpawnEffect("Wm_mr_wallkick_l", 0, &efPos, &(S16Vec){0,0,0}, &(Vec){2.0, 2.0, 2.0});
	}

	if (chrAnimation.isAnimationDone()) {
		doStateChange(&StateID_Outro);
	}
}

void daCaptainBowser::endState_FinalAttack() {
}

void daCaptainBowser::beginState_Outro() {
	WLClass::instance->_4 = 5;
	WLClass::instance->_8 = 0;
	dStage32C_c::instance->freezeMarioBossFlag = 1;

	bowserX += 56.0f;
	bindAnimChr_and_setUpdateRate("kp_death1", 1, 0.0, 1.0f); 

	//shipRotY = -0x4000;
	shipAnm.bind(&shipModel, shipFile.GetResAnmChr("mastfail_after"), 1);
	shipModel.bindAnim(&shipAnm, 0.0);
	shipAnm.setUpdateRate(0.5f);

	bowserXSpeed = -3.5f;
	bowserYSpeed = 8.9f;
	bowserMaxYSpeed = -4.0f;
	bowserYAccel = -0.24375f;

	nw4r::snd::SoundHandle handle;
	PlaySoundWithFunctionB4(SoundRelatedClass, &handle, SE_VOC_KP_L_FALL, 1);

	explosionBottomBound = 0.0f;

	timer = 0;
	timerWhenCrashHappens = 1000000;
	timerForVictoryDance = 1000000;
}

extern void *_8042A788;
extern void playFanfare(void *, int type);
extern dStateBase_c JrClownEndDemoState;
void daCaptainBowser::executeState_Outro() { 
	timer++;

	if (!shipAnmFinished) {
		float frame = shipAnm.getCurrentFrame();

		// nuke the things!
		if (frame == 35.0f) {
			dActor_c *iter = 0;

			while (iter = (dActor_c*)dActor_c::searchByBaseType(2, iter)) {
				dStageActor_c *sa = (dStageActor_c*)iter;

				if (sa->name == EN_BIRIKYU_MAKER || sa->name == KAZAN_MGR) {
					sa->Delete(1);
				}

				if (sa->name == EN_LINE_BIRIKYU ||
						sa->name == EN_STAR_COIN ||
						sa->name == EN_HATENA_BALLOON ||
						sa->name == EN_ITEM ||
						sa->name == EN_TARZANROPE || // Meteor
						sa->name == WM_ANCHOR) { // Koopa Throw
					sa->killedByLevelClear();
					sa->Delete(1);
				}
			}

			// freeze ye olde clowne
			dEn_c *clownIter = 0;
			while (clownIter = (dEn_c*)dEn_c::search(JR_CLOWN_FOR_PLAYER, clownIter)) {
				clownIter->doStateChange(&JrClownEndDemoState);
			}
			forceClownEnds = true;

			// remove all ship collisions
			for (int i = 0; i < SHIP_SCOLL_COUNT; i++)
				shipSColls[i].removeFromList();

		} else if (frame == 150.0f) {
			nw4r::snd::SoundHandle handle;
			PlaySoundWithFunctionB4(SoundRelatedClass, &handle, SE_BOSS_KOOPA_CRASH, 1);
			timerWhenCrashHappens = timer;
			prplSound.Stop(10);

			ClassWithCameraInfo *cwci = ClassWithCameraInfo::instance;
			Vec efPos = {cwci->screenCentreX+168.0f, cwci->screenTop-cwci->screenHeight, pos.z};
			SpawnEffect("Wm_bg_volcano", 0, &efPos, &(S16Vec){0,0,0}, &(Vec){4.0, 4.0, 4.0});

			ShakeScreen(StageScreen, 17, 7, 0, 0);

		} else if (shipAnm.isAnimationDone()) {
			shipAnmFinished = true;
		}

		if (frame > 30.0f) {
			if (timer & 4 && explosionBottomBound > -249.0f) {
				static const char *efs[] = {"Wm_en_explosion", "Wm_ob_cmnboxpiece"};

				int id = MakeRandomNumber(2);
				Vec efPos = {
					pos.x - 150.0f + MakeRandomNumber(300),
					pos.y + explosionBottomBound + MakeRandomNumber(250 + explosionBottomBound),
					pos.z + 200.0f
				};
				float efScale = (float(MakeRandomNumber(30)) / 20.0f);

				SpawnEffect(efs[id], 0, &efPos, &(S16Vec){0,0,0}, &(Vec){efScale,efScale,efScale});
			}

			if ((timer % 12) == 0 && (frame < 150.0f)) {
				nw4r::snd::SoundHandle handle;
				PlaySoundWithFunctionB4(SoundRelatedClass, &handle, SE_DEMO_OP_CAKE_CLASH_1210f, 1);
			}

			explosionBottomBound -= 0.85f;
		}

		// handleYSpeed():
		bowserYSpeed += bowserYAccel;
		if (bowserYSpeed < bowserMaxYSpeed)
			bowserYSpeed = bowserMaxYSpeed;

		// doMovement()
		bowserX += bowserXSpeed;
		bowserY += bowserYSpeed;

		bowserRotX -= 0xC00;
		bowserRotY -= 0x100;
	}


	// FUN SHITS.
	if (timer == (timerWhenCrashHappens + 90)) {
		playFanfare(_8042A788, 4);
		timerForVictoryDance = timer + (7*60);

		// Prepare for moving the clowns
		clownCount = 0;

		dEn_c *unsortedClownPointers[4];
		dAcPy_c *unsortedClownPlayers[4];

		dEn_c *clownIter = 0;
		while (clownIter = (dEn_c*)dEn_c::search(JR_CLOWN_FOR_PLAYER, clownIter)) {
			dAcPy_c *player = *((dAcPy_c**)(((u32)clownIter) + 0x738));
			if (player) {
				unsortedClownPointers[clownCount] = clownIter;
				unsortedClownPlayers[clownCount] = player;
				clownCount++;
			}
		}

		// Now sort them by ID
		int sortedClownID = 0;
		for (int playerID = 0; playerID < 4; playerID++) {
			dAcPy_c *player = (dAcPy_c*)GetSpecificPlayerActor(playerID);

			for (int searchClown = 0; searchClown < clownCount; searchClown++) {
				if (player == unsortedClownPlayers[searchClown]) {
					clownPointers[sortedClownID] = unsortedClownPointers[searchClown];
					sortedClownID++;
					break;
				}
			}
		}

		ClassWithCameraInfo *cwci = ClassWithCameraInfo::instance;

		// calculate their end positions
		// Assuming each clown takes up ~32 width, and we have 16 padding between them ...
		float clownSpan = (clownCount * 48.0f) - 16.0f;
		float clownX = cwci->screenCentreX - (clownSpan / 2.0f);

		for (int i = 0; i < clownCount; i++) {
			clownDests[i].x = clownX;
			clownDests[i].y = cwci->screenCentreY;
			clownX += 48.0f;
		}
	}

	if (timer > (timerWhenCrashHappens + 90)) {
		// Let's move the clowns
		int playersAtEnd = 0;

		for (int i = 0; i < clownCount; i++) {
			dEn_c *clown = clownPointers[i];

			float moveSpeed = 1.2f;
			
			// Are we there already?
			float xDiff = abs(clown->pos.x - clownDests[i].x);
			float yDiff = abs(clown->pos.y - clownDests[i].y);

			int check = 0;
			if (xDiff < moveSpeed) {
				clown->pos.x = clownDests[i].x;
				check |= 1;
			}
			if (yDiff < moveSpeed) {
				clown->pos.y = clownDests[i].y;
				check |= 2;
			}
			if (check == 3) {
				playersAtEnd++;
				continue;
			}

			// Otherwise, move them a bit further
			VEC3 direction = {clownDests[i].x - clown->pos.x, clownDests[i].y - clown->pos.y, 0.0f};
			VECNormalize(&direction, &direction);
			VECScale(&direction, &direction, moveSpeed);

			clown->pos.x += direction.x;
			clown->pos.y += direction.y;
		}

		if (playersAtEnd >= clownCount) {
			//if (timerForVictoryDance == 1000000)
			//	timerForVictoryDance = timer + 45;
		}

		if (timer == timerForVictoryDance) {
			static const int vocs[4] = {
				SE_VOC_MA_CLEAR_LAST_BOSS,
				SE_VOC_LU_CLEAR_LAST_BOSS,
				SE_VOC_KO_CLEAR_LAST_BOSS,
				SE_VOC_KO2_CLEAR_LAST_BOSS
			};
			for (int i = 0; i < clownCount; i++) {
				dAcPy_c *player = *((dAcPy_c**)(((u32)clownPointers[i]) + 0x738));
				// let's be hacky
				dPlayerModelHandler_c *pmh = (dPlayerModelHandler_c*)(((u32)player) + 0x2A60);
				int whatAnim = goal_puton_capB;
				if (pmh->mdlClass->powerup_id == 4)
					whatAnim = goal_puton_capB;
				else if (pmh->mdlClass->powerup_id == 5)
					whatAnim = goal_puton_capC;
				pmh->mdlClass->startAnimation(whatAnim, 1.0f, 0.0f, 0.0f);

				nw4r::snd::SoundHandle handle;
				PlaySoundWithFunctionB4(SoundRelatedClass, &handle, vocs[pmh->mdlClass->player_id_1], 1);
			}
		}

		if (timer == (timerForVictoryDance + 180)) {
			doStateChange(&StateID_PanToExit);
			nw4r::snd::SoundHandle handle;
			PlaySoundWithFunctionB4(SoundRelatedClass, &handle, SE_PLY_CROWN_ACC, 1);
		}
		//dAcPy_c *splayer = *((dAcPy_c**)(((u32)clownPointers[0]) + 0x738));
		//OSReport("Player has %s\n", splayer->states2.getCurrentState()->getName());
	}
}
void daCaptainBowser::endState_Outro() {  }


void daCaptainBowser::beginState_PanToExit() {
	timer = 0;
}
void daCaptainBowser::endState_PanToExit() {
}

void daCaptainBowser::executeState_PanToExit() {
	float targetClownY = ClassWithCameraInfo::instance->screenCentreY - 160.0f;

	for (int i = 0; i < clownCount; i++) {
		dEn_c *clown = clownPointers[i];
		clown->pos.y -= 1.5f;
		if (clown->pos.y < targetClownY && !exitedFlag) {
			RESTART_CRSIN_LevelStartStruct.purpose = 0;
			RESTART_CRSIN_LevelStartStruct.world1 = 7;
			RESTART_CRSIN_LevelStartStruct.world2 = 7;
			RESTART_CRSIN_LevelStartStruct.level1 = 40;
			RESTART_CRSIN_LevelStartStruct.level2 = 40;
			RESTART_CRSIN_LevelStartStruct.areaMaybe = 0;
			RESTART_CRSIN_LevelStartStruct.entrance = 0xFF;
			RESTART_CRSIN_LevelStartStruct.unk4 = 0; // load replay
			DontShowPreGame = true;
			ExitStage(RESTART_CRSIN, 0, BEAT_LEVEL, MARIO_WIPE);
			exitedFlag = true;

			for (int i = 0; i < 4; i++)
				if (Player_Lives[i] == 0)
					Player_Lives[i] = 5;
		}
	}
}


//
// processed\../src/bossKoopaThrow.cpp
//

#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include "boss.h"

struct TypeInfo { 
	const char *arcName;
	const char *brresName; 
	const char *modelName; 
	const char *deathEffect; 
	int launchSound; 
	int breakSound; 
	int flySound; 
	float size; 
	float scale; 
	u16 xrot; 
	u16 yrot; 
	u16 zrot; 
};

static const TypeInfo types[6] = {
	{"choropoo", 			"g3d/choropoo.brres", 			"spanner", 				"Wm_en_hit", 		0, 						SE_BOSS_JR_FLOOR_BREAK, 0, 							8.0f, 	2.0f, 0, 		0, 		0x1000},
	{"choropoo", 			"g3d/choropoo.brres", 			"spanner", 				"Wm_en_burst_s", 	0, 						SE_BOSS_JR_BOMB_BURST, 	0, 							12.0f, 	2.0f, 0, 		0, 		0x1000},
	{"koopa_clown_bomb", 	"g3d/koopa_clown_bomb.brres", 	"koopa_clown_bomb", 	"Wm_en_burst_s", 	SE_EMY_ELCJ_THROW, 		SE_BOSS_JR_BOMB_BURST, 	0, 							16.0f, 	0.8f, 0x200, 	0x800, 	0x1000},
	{"bros", 				"g3d/t00.brres", 				"bros_hammer", 			"Wm_en_hit", 		0, 						SE_OBJ_HAMMER_HIT_BOTH, 0,						 	16.0f, 	2.0f, 0, 		0, 		0x1000},
	{"dossun",				"g3d/t02.brres", 				"dossun", 				"Wm_en_hit", 		SE_EMY_DOSSUN, 			SE_EMY_DOSSUN_DEAD, 	0, 							14.0f, 	1.0f, 0,		0,		0},
	{"KoopaShip",			"g3d/present.brres", 			"PresentBox_penguin", 	"Wm_dm_presentopen",SE_DEMO_OP_PRESENT_THROW_2400f, SE_DEMO_OP_PRESENT_BOX_BURST, 	0, 			20.0f, 	1.0f, 0x20, 	0x40, 	0x200}
};


const char* KPTarcNameList [] = {
	"choropoo",
	"koopa_clown_bomb",
	"dossun",
	"KoopaShip",
	NULL	
};


class daKoopaThrow : public dEn_c {
	int onCreate();
	int onExecute();
	int onDelete();
	int onDraw();

	mHeapAllocator_c allocator;
	m3d::mdl_c bodyModel;

	int timer;
	char Type;
	char direction;
	char front;
	float ymod;
	int lifespan;
	u32 cmgr_returnValue;

	bool playsAnim;
	m3d::anmChr_c chrAnim;

	nw4r::snd::SoundHandle hammerSound;

	const TypeInfo *currentInfo;

	static daKoopaThrow *build();

	void updateModelMatrices();
	void playerCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	void spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther);

	bool collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther);


	USING_STATES(daKoopaThrow);
	DECLARE_STATE(Straight);

};

CREATE_STATE(daKoopaThrow, Straight);


// Types:
//
//	0 - Wrench
//  1 - Exploding Wrench
//	2 - Bomb
//  3 - Hammer
//  4 - Thwomp
//	5 - Present
//


extern "C" void *PlayWrenchSound(dEn_c *);
extern "C" void *dAcPy_c__ChangePowerupWithAnimation(void * Player, int powerup);
extern "C" int CheckExistingPowerup(void * Player);

void daKoopaThrow::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther) { 
	if (Type == 5) {
		PlaySoundAsync(this, currentInfo->breakSound);
		SpawnEffect(currentInfo->deathEffect, 0, &this->pos, &(S16Vec){0,0,0}, &(Vec){3.0, 3.0, 3.0});
		// dStageActor_c *spawned = CreateActor(EN_ITEM, 0x20000063, this->pos, 0, 0);
		// spawned->pos.x = this->pos.x;
		// spawned->pos.y = this->pos.y;

		int p = CheckExistingPowerup(apOther->owner);
		if (p == 0 || p == 3) {	// Powerups - 0 = small; 1 = big; 2 = fire; 3 = mini; 4 = prop; 5 = peng; 6 = ice; 7 = hammer
			dAcPy_c__ChangePowerupWithAnimation(apOther->owner, 1);
		}

		this->Delete(1);

		return;
	}

	DamagePlayer(this, apThis, apOther);

	if (Type == 1 || Type == 2) {
		PlaySoundAsync(this, SE_BOSS_JR_BOMB_BURST);

		SpawnEffect("Wm_en_burst_s", 0, &this->pos, &(S16Vec){0,0,0}, &(Vec){0.75, 0.75, 0.75});
		SpawnEffect("Wm_mr_wirehit", 0, &this->pos, &(S16Vec){0,0,0}, &(Vec){1.25, 1.25, 1.25});
		this->Delete(1);
	}
}

void daKoopaThrow::spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther) {}

bool daKoopaThrow::collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther) {
	return true;
}
bool daKoopaThrow::collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther) { 
	return false; 
}
bool daKoopaThrow::collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther) {
	return true;
}
bool daKoopaThrow::collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther) {
	if (Type == 1 || Type == 2) {
		SpawnEffect("Wm_en_burst_s", 0, &this->pos, &(S16Vec){0,0,0}, &(Vec){0.75, 0.75, 0.75});
		SpawnEffect("Wm_mr_wirehit", 0, &this->pos, &(S16Vec){0,0,0}, &(Vec){1.25, 1.25, 1.25});
	}
	else {
		SpawnEffect("Wm_ob_cmnboxgrain", 0, &this->pos, &(S16Vec){0,0,0}, &(Vec){0.5, 0.5, 0.5});		
	}

	PlaySoundAsync(this, currentInfo->breakSound);
	this->Delete(1);
	return true;
}
bool daKoopaThrow::collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther) {
	return true;
}
bool daKoopaThrow::collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther) { 
	DamagePlayer(this, apThis, apOther);

	if (Type == 1 || Type == 2) {
		PlaySoundAsync(this, SE_BOSS_JR_BOMB_BURST);

		SpawnEffect("Wm_en_burst_s", 0, &this->pos, &(S16Vec){0,0,0}, &(Vec){0.75, 0.75, 0.75});
		SpawnEffect("Wm_mr_wirehit", 0, &this->pos, &(S16Vec){0,0,0}, &(Vec){1.25, 1.25, 1.25});
		this->Delete(1);
	}
	return true;
}


daKoopaThrow *daKoopaThrow::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daKoopaThrow));
	return new(buffer) daKoopaThrow;
}


int daKoopaThrow::onCreate() {
	
	this->direction = this->settings & 0xF;
	this->Type = (this->settings >> 4) & 0xF;
	this->front = (this->settings >> 8) & 0xF;

	currentInfo = &types[Type]; 

	allocator.link(-1, GameHeaps[0], 0, 0x20);

	nw4r::g3d::ResFile rf(getResource(currentInfo->arcName, currentInfo->brresName));
	nw4r::g3d::ResMdl resMdl = rf.GetResMdl(currentInfo->modelName);

	bodyModel.setup(resMdl, &allocator, (Type == 4 ? 0x224 : 0), 1, 0);
	SetupTextures_Enemy(&bodyModel, 0);

	if (Type == 4) {
		// Thwomp
		playsAnim = true;

		nw4r::g3d::ResAnmChr anmChr = rf.GetResAnmChr("boss_throw");
		chrAnim.setup(resMdl, anmChr, &allocator, 0);
		chrAnim.bind(&bodyModel, anmChr, 1);
		bodyModel.bindAnim(&chrAnim, 0.0);
		chrAnim.setUpdateRate(1.0);
	}

	allocator.unlink();
	
	
	ActivePhysics::Info KoopaJunk;
	
	KoopaJunk.xDistToCenter = 0.0f;
	KoopaJunk.yDistToCenter = (Type == 4) ? currentInfo->size : 0.0;
	KoopaJunk.xDistToEdge = currentInfo->size;
	KoopaJunk.yDistToEdge = currentInfo->size;

	this->scale.x = currentInfo->scale;
	this->scale.y = currentInfo->scale;
	this->scale.z = currentInfo->scale;
	
	KoopaJunk.category1 = 0x3;
	KoopaJunk.category2 = 0x0;
	KoopaJunk.bitfield1 = 0x47;
	KoopaJunk.bitfield2 = 0xFFFFFFFF;
	KoopaJunk.unkShort1C = 0;
	KoopaJunk.callback = &dEn_c::collisionCallback;

	this->aPhysics.initWithStruct(this, &KoopaJunk);
	this->aPhysics.addToList();


	spriteSomeRectX = currentInfo->size;
	spriteSomeRectY = currentInfo->size;
	_320 = 0.0f;
	_324 = currentInfo->size;

	// These structs tell stupid collider what to collide with - these are from koopa troopa
	static const lineSensor_s below(12<<12, 4<<12, 0<<12);
	static const pointSensor_s above(0<<12, 12<<12);
	static const lineSensor_s adjacent(6<<12, 9<<12, 6<<12);

	collMgr.init(this, &below, &above, &adjacent);
	collMgr.calculateBelowCollisionWithSmokeEffect();

	cmgr_returnValue = collMgr.isOnTopOfTile();


	if (this->direction == 0) 	   { // Ground Facing Left
		this->pos.x -= 0.0; // -32 to +32
		this->pos.y += 36.0;
		// this->rot.z = 0x2000;
	}
	else if (this->direction == 1) { // Ground Facing Right
		this->pos.x += 0.0; // +32 to -32
		this->pos.y += 36.0;
		// this->rot.z = 0xE000;
	}
	if (this->front == 1) { this->pos.z = -1804.0; }
	else 				  { this->pos.z =  3300.0; }

	
	if (currentInfo->launchSound != 0) {
		PlaySound(this, currentInfo->launchSound);
	}

	if (Type == 3) {
		PlaySoundWithFunctionB4(SoundRelatedClass, &hammerSound, SE_EMY_MEGA_BROS_HAMMER, 1);
	}

	doStateChange(&StateID_Straight);
	
	this->onExecute();
	return true;
}


int daKoopaThrow::onDelete() {
	if (hammerSound.Exists())
		hammerSound.Stop(10);
	return true;
}

int daKoopaThrow::onDraw() {
	bodyModel.scheduleForDrawing();
	return true;
}


void daKoopaThrow::updateModelMatrices() {
	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	bodyModel.setDrawMatrix(matrix);
	bodyModel.setScale(&scale);
	bodyModel.calcWorld(false);
}


int daKoopaThrow::onExecute() {
	acState.execute();
	updateModelMatrices();
	if (playsAnim) {
		if (chrAnim.isAnimationDone())
			chrAnim.setCurrentFrame(0.0f);
		bodyModel._vf1C();
	}

	float rect[] = {this->_320, this->_324, this->spriteSomeRectX, this->spriteSomeRectY};
	int ret = this->outOfZone(this->pos, (float*)&rect, this->currentZoneID);
	if(ret) {
		this->Delete(1);
	}

	return true;
}



void daKoopaThrow::beginState_Straight() { 
	float rand = (float)GenerateRandomNumber(10) * 0.4;

	if (this->direction == 0) { // directions 1 spins clockwise, fly rightwards
		speed.x = 1.5 + rand; 
	}
	else {						// directions 0 spins anti-clockwise, fly leftwards
		speed.x = -1.5 - rand; 
	}

	speed.y = 9.0;
}
void daKoopaThrow::executeState_Straight() { 

	speed.y = speed.y - 0.01875;

	HandleXSpeed();
	HandleYSpeed();
	doSpriteMovement();

	// cmgr_returnValue = collMgr.isOnTopOfTile();
	// collMgr.calculateBelowCollisionWithSmokeEffect();

	// if (collMgr.isOnTopOfTile() || (collMgr.outputMaybe & (0x15 << direction))) {
	// 	// hit the ground or wall
	// 	PlaySoundAsync(this, currentInfo->breakSound);

	// 	SpawnEffect(currentInfo->deathEffect, 0, &this->pos, &(S16Vec){0,0,0}, &(Vec){0.75, 0.75, 0.75});
	// 	this->Delete(1);
	// }

	if (this->direction == 1) { // directions 1 spins clockwise, fly rightwards
		this->rot.x -= currentInfo->xrot;
		this->rot.y -= currentInfo->yrot;
		this->rot.z -= currentInfo->zrot; }
	else {						// directions 0 spins anti-clockwise, fly leftwards
		this->rot.x -= currentInfo->xrot;
		this->rot.y -= currentInfo->yrot;
		this->rot.z += currentInfo->zrot; }	
	

	if (Type < 2) {
		PlayWrenchSound(this);
	}
	else if (currentInfo->flySound == 0) { return; }
	else {
		PlaySound(this, currentInfo->flySound);
	}

}
void daKoopaThrow::endState_Straight() { }










//
// processed\../src/bossFlameThrower.cpp
//

#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include "boss.h"

class daKoopaBreath : public dEn_c {
	int onCreate();
	int onExecute();

	mEf::es2 effect;
	static daKoopaBreath *build();

	void playerCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	void spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther);

};

void daKoopaBreath::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther) { DamagePlayer(this, apThis, apOther); }
void daKoopaBreath::spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther) {}


daKoopaBreath *daKoopaBreath::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daKoopaBreath));
	return new(buffer) daKoopaBreath;
}


int daKoopaBreath::onCreate() {
	
	ActivePhysics::Info GreatBalls;
	
	GreatBalls.xDistToCenter = 0.0;
	GreatBalls.yDistToCenter = 0.0;
	GreatBalls.xDistToEdge = 38.0;
	GreatBalls.yDistToEdge = 28.0;
	
	GreatBalls.category1 = 0x3;
	GreatBalls.category2 = 0x0;
	GreatBalls.bitfield1 = 0x47;
	GreatBalls.bitfield2 = 0xFFFFFFFF;
	GreatBalls.unkShort1C = 0;
	GreatBalls.callback = &dEn_c::collisionCallback;

	this->aPhysics.initWithStruct(this, &GreatBalls);
	this->aPhysics.addToList();
	speed.x = -2.0; 
	
	this->onExecute();
	return true;
}


int daKoopaBreath::onExecute() {
	HandleXSpeed();
	doSpriteMovement();

	PlaySound(this, SE_BOSS_JR_FIRE_BURNING);

	effect.spawn("Wm_ko_fireattack", 0, &(Vec){pos.x, pos.y, pos.z}, &(S16Vec){0,0,0}, &(Vec){3.0, 3.0, 3.0});

	float rect[] = {0.0, 0.0, 38.0, 38.0};
	int ret = this->outOfZone(this->pos, (float*)&rect, this->currentZoneID);
	if(ret) {
		this->Delete(1);
	}

	return true;
}

//
// processed\../src/bossBridgeBowser.cpp
//

#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include <stage.h>
#include "boss.h"

extern "C" void *BowserExitDemoState(void *, unsigned int);
extern "C" void *ForceMarioExitDemoMode(void *, unsigned int);
extern "C" void *BowserFireballCollision(dEn_c *, ActivePhysics *, ActivePhysics *);
extern "C" void *BowserDamageAnmClr(dEn_c *);
extern "C" void *BowserDamageStepTwo(dEn_c *);
extern "C" void *BowserDamageNormal(dEn_c *);
extern "C" void *BowserDamageKill(dEn_c *);
extern "C" void *BowserDamageEnd(dEn_c *);

int BridgeBowserHP = 2;
int lastBomb = 0;

extern bool HackyBombDropVariable;

void BowserDoomSpriteCollision(dEn_c *bowser, ActivePhysics *apThis, ActivePhysics *apOther) {
	// If you collide with something or other, call the fireball collision

	if (apOther->owner->name == 674) {

		if (lastBomb == apOther->owner->id) { return; }
		if (!HackyBombDropVariable) return;
		HackyBombDropVariable = false;

		// void * bowserClass = (void*)(((u32)bowser) + 0x5F8);
		// int HP = *(int*)(((u32)bowserClass) + 4);

		OSReport("HP: %d", BridgeBowserHP);

		if (BridgeBowserHP <= 0) {
			BridgeBowserHP = 0;

			*(int*)(((u32)bowser) + 0x540) = 0x28;

			BowserDamageAnmClr(bowser);

			BowserDamageStepTwo(bowser);
			BowserDamageKill(bowser);

			// WeirdLevelEndClass->sub_8005CB60(*otherActor->returnPtrToField38D());

			// this->vf300(otherActor);
			BowserDamageEnd(bowser);

			// daBossKoopaDemo_c *BowserDemo = (daBossKoopaDemo_c*)FindActorByType(BOSS_KOOPA_DEMO, 0);
			daBossKoopa_c *BowserClass = (daBossKoopa_c*)bowser;
			OSReport("Koopa Controller: %x", BowserClass);
			BowserClass->doStateChange(&daBossKoopa_c::StateID_Fall);	
			dFlagMgr_c::instance->set(3, 0, true, false, false);

			BridgeBowserHP = 2;

		}
		else {
			*(int*)(((u32)bowser) + 0x540) = 0x28;

			BowserDamageAnmClr(bowser);
			BowserDamageNormal(bowser);

			BridgeBowserHP -= 1;
		}

		lastBomb = apOther->owner->id;

		dEn_c * bomb = (dEn_c*)apOther->owner;
		dFlagMgr_c::instance->set(bomb->settings & 0xFF, 0, true, false, false);
		bomb->kill();
	}

	return;
}

void BowserDoomStart(dStageActor_c *Controller) {
	OSReport("Here we go!");

	dEn_c *Bowser = (dEn_c*)FindActorByType(EN_BOSS_KOOPA, 0);
	Bowser->Delete(1);
	lastBomb = 0;
}

void BowserDoomExecute(dStageActor_c *Controller) {
	dFlagMgr_c::instance->set(2, 0, true, false, false);
	Controller->Delete(1);
}

void BowserDoomEnd(dStageActor_c *Controller) {
	OSReport("Bai bai everybody");
	Controller->Delete(1);
}

void BowserStartEnd(dStageActor_c *Controller) {
	dFlagMgr_c::instance->set(1, 0, true, false, false);
}

//
// processed\../src/bossBombDrop.cpp
//

#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include "boss.h"


const char* BDarcNameList [] = {
	"koopa_clown_bomb",
	NULL	
};


class dDroppedBomb : public dEn_c {
	int onCreate();
	int onExecute();
	int onDelete();
	int onDraw();

	void kill();

	mHeapAllocator_c allocator;
	m3d::mdl_c bodyModel;

	u32 cmgr_returnValue;

	static dDroppedBomb *build();

	void updateModelMatrices();
	void playerCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	// void spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther);

	bool collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther);
};

void dDroppedBomb::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther) { 
	DamagePlayer(this, apThis, apOther);
	this->kill();
}

dDroppedBomb *dDroppedBomb::build() {
	void *buffer = AllocFromGameHeap1(sizeof(dDroppedBomb));
	return new(buffer) dDroppedBomb;
}

// void dDroppedBomb::spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther) { this->kill(); }
bool dDroppedBomb::collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther) {return true;}
bool dDroppedBomb::collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther) { return false; }
bool dDroppedBomb::collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther) {return true;}
bool dDroppedBomb::collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther) {return true;}
bool dDroppedBomb::collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther) {return true;}
bool dDroppedBomb::collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther) {return true;}

void dDroppedBomb::kill() {
	PlaySoundAsync(this, SE_BOSS_JR_BOMB_BURST);

	SpawnEffect("Wm_en_explosion", 0, &this->pos, &(S16Vec){0,0,0}, &(Vec){1.0, 1.0, 1.0});
	SpawnEffect("Wm_mr_wirehit", 0, &this->pos, &(S16Vec){0,0,0}, &(Vec){1.25, 1.25, 1.25});
	this->Delete(1);	
}

int dDroppedBomb::onCreate() {
	
	allocator.link(-1, GameHeaps[0], 0, 0x20);

	nw4r::g3d::ResFile rf(getResource("koopa_clown_bomb", "g3d/koopa_clown_bomb.brres"));
	bodyModel.setup(rf.GetResMdl("koopa_clown_bomb"), &allocator, 0x224, 1, 0);
	SetupTextures_Enemy(&bodyModel, 0);

	allocator.unlink();
	
	
	ActivePhysics::Info KoopaJunk;
	
	KoopaJunk.xDistToCenter = -20.0f;
	KoopaJunk.yDistToCenter = 0.0;
	KoopaJunk.xDistToEdge = 20.0f;
	KoopaJunk.yDistToEdge = 20.0f;

	this->scale.x = 1.0;
	this->scale.y = 1.0;
	this->scale.z = 1.0;
	
	KoopaJunk.category1 = 0x3;
	KoopaJunk.category2 = 0x0;
	KoopaJunk.bitfield1 = 0x4F;
	KoopaJunk.bitfield2 = 0xFFFFFFFF;
	KoopaJunk.unkShort1C = 0;
	KoopaJunk.callback = &dEn_c::collisionCallback;

	this->aPhysics.initWithStruct(this, &KoopaJunk);
	this->aPhysics.addToList();


	spriteSomeRectX = 20.f;
	spriteSomeRectY = 20.f;
	_320 = 0.0f;
	_324 = 20.f;

	// These structs tell stupid collider what to collide with - these are from koopa troopa
	static const lineSensor_s below(12<<12, 4<<12, 0<<12);
	static const pointSensor_s above(0<<12, 12<<12);
	static const lineSensor_s adjacent(6<<12, 9<<12, 6<<12);

	collMgr.init(this, &below, &above, &adjacent);
	collMgr.calculateBelowCollisionWithSmokeEffect();

	cmgr_returnValue = collMgr.isOnTopOfTile();


	pos.z = 3300.0;
	speed.y = -1.5f;

	PlaySound(this, SE_EMY_ELCJ_THROW);
	return true;
}


int dDroppedBomb::onDelete() {
	return true;
}

int dDroppedBomb::onDraw() {
	bodyModel.scheduleForDrawing();
	return true;
}


void dDroppedBomb::updateModelMatrices() {
	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	bodyModel.setDrawMatrix(matrix);
	bodyModel.setScale(&scale);
	bodyModel.calcWorld(false);
}


int dDroppedBomb::onExecute() {
	// acState.execute();
	updateModelMatrices();

	rot.x += 0x200;
	rot.y += 0x400;
	rot.z += 0x600;

	float rect[] = {this->_320, this->_324, this->spriteSomeRectX, this->spriteSomeRectY};
	int ret = this->outOfZone(this->pos, (float*)&rect, this->currentZoneID);
	if(ret) {
		this->Delete(1);
		dFlagMgr_c::instance->set(settings & 0xFF, 0, true, false, false);
	}

	speed.y = speed.y - 0.01875;

	HandleXSpeed();
	HandleYSpeed();
	doSpriteMovement();

	cmgr_returnValue = collMgr.isOnTopOfTile();
	collMgr.calculateBelowCollisionWithSmokeEffect();

	if (collMgr.isOnTopOfTile() || (collMgr.outputMaybe & (0x15 << direction))) {
		this->kill();
		dFlagMgr_c::instance->set(settings & 0xFF, 0, true, false, false);
	}

	return true;
}





// =========================================================================================================
// ================================================ CONTROLLER =============================================
// =========================================================================================================

class dBombDrop : public dStageActor_c {
	int onCreate();
	int onExecute();
	int onDelete();
	int onDraw();

	int timer;
	dStageActor_c * target;
	int eventA;
	int eventB;

	static dBombDrop *build();
};

dBombDrop *dBombDrop::build() {
	void *buffer = AllocFromGameHeap1(sizeof(dBombDrop));
	return new(buffer) dBombDrop;
}


bool HackyBombDropVariable = false;
extern int BridgeBowserHP;

int dBombDrop::onCreate() {
	BridgeBowserHP = 2;
	
	int t = this->settings & 0xF;
	this->eventA = ((this->settings >> 24) & 0xFF) - 1;
	this->eventB = ((this->settings >> 16) & 0xFF) - 1;


	if (t == 0) {
		target = (dStageActor_c*)FindActorByType(EN_BOSS_KOOPA, 0);
	}
	else {
		target = GetSpecificPlayerActor(t - 1);
	}

	dFlagMgr_c::instance->set(eventA, 0, false, false, false);
	dFlagMgr_c::instance->set(eventB, 0, false, false, false);

	HackyBombDropVariable = false;
	
	return true;
}

int dBombDrop::onDelete() { return true; }
int dBombDrop::onDraw() { return true; }

int dBombDrop::onExecute() {
	pos.x = target->pos.x;

	bool active;
	active = dFlagMgr_c::instance->active(eventA);
	if (active) {
		create(WM_SMALLCLOUD, eventA+1, &pos , &rot, 0);
		HackyBombDropVariable = true;
		dFlagMgr_c::instance->set(eventA, 0, false, false, false);
	}

	active = dFlagMgr_c::instance->active(eventB);
	if (active) {
		create(WM_SMALLCLOUD, eventB+1, &pos, &rot, 0);
		HackyBombDropVariable = true;
		dFlagMgr_c::instance->set(eventB, 0, false, false, false);
	}

	return true;
}






//
// processed\../src/effectvideo.cpp
//

#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>


extern "C" bool SpawnEffect(const char*, int, Vec*, S16Vec*, Vec*);


class EffectVideo : public dEn_c {
	int onCreate();
	int onExecute();
	int onDelete();

	u64 eventFlag;
	s32 timer;
	u32 delay;

	u32 effect;
	u8 type;
	float scale;

	static EffectVideo *build();

};


EffectVideo *EffectVideo::build() {
	void *buffer = AllocFromGameHeap1(sizeof(EffectVideo));
	return new(buffer) EffectVideo;
}


int EffectVideo::onCreate() {
	
	this->timer = 0;

	char eventNum	= (this->settings >> 24) & 0xFF;

	this->eventFlag = (u64)1 << (eventNum - 1);
	
	this->type		= (this->settings >> 16) & 0xF;
	this->effect	= this->settings & 0xFFF;
	this->scale		= float((this->settings >> 20) & 0xF) / 4.0;
	this->delay		= (this->settings >> 12) & 0xF * 30;
	
	if (this->scale == 0.0) { this->scale = 1.0; }

	this->onExecute();
	return true;
}


int EffectVideo::onDelete() {
	return true;
}


int EffectVideo::onExecute() {

	if (dFlagMgr_c::instance->flags & this->eventFlag) {

		if (this->timer == this->delay) {

			if (this->type == 0) { // Plays a sound
				PlaySoundAsync(this, this->effect);
			}
	
			else {	// Plays an Effect

				const char *efName = 0;

				switch (this->effect) {
					case 1: efName = "Wm_mr_cmnsndlandsmk"; break;
					case 3: efName = "Wm_en_landsmoke"; break;
					case 5: efName = "Wm_en_sndlandsmk"; break;
					case 6: efName = "Wm_en_sndlandsmk_s"; break;
					case 43: efName = "Wm_ob_cmnspark"; break;
					case 159: efName = "Wm_mr_hardhit_grain"; break;
					case 177: efName = "Wm_ob_greencoinkira_b"; break;
					case 193: efName = "Wm_mr_electricshock_biri01_s"; break;
					case 216: efName = "Wm_en_blockcloud"; break;
					case 365: efName = "Wm_en_kuribobigsplit"; break;
					case 514: efName = "Wm_ob_fireworks_y"; break;
					case 517: efName = "Wm_ob_fireworks_b"; break;
					case 520: efName = "Wm_ob_fireworks_g"; break;
					case 523: efName = "Wm_ob_fireworks_p"; break;
					case 526: efName = "Wm_ob_fireworks_k"; break;
					case 533: efName = "Wm_ob_fireworks_1up"; break;
					case 540: efName = "Wm_ob_fireworks_star"; break;
					case 843: efName = "Wm_en_explosion_smk"; break;
					case 864: efName = "Wm_en_obakedoor_sm"; break;
					case 865: efName = "Wm_en_obakedoor_ic"; break;
				}

				if (efName != 0)
					SpawnEffect(efName, 0, &this->pos, &(S16Vec){0,0,0}, &(Vec){this->scale, this->scale, this->scale});
			}
	
			this->timer = 0;
			if (this->delay == 0) { this->delay = -1; }
		}
		
		this->timer += 1;
	}
	return true;
}


//
// processed\../src/fakeStarCoin.cpp
//

#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>


extern "C" bool SpawnEffect(const char*, int, Vec*, S16Vec*, Vec*);


class daFakeStarCoin : public dEn_c {
	int onCreate();
	int onExecute();
	int onDelete();
	int onDraw();

	mHeapAllocator_c allocator;
	m3d::mdl_c bodyModel;

	u64 eventFlag;
	s32 timer;
	u32 delay;

	u32 effect;
	u8 type;

	static daFakeStarCoin *build();

	void updateModelMatrices();
	void playerCollision(ActivePhysics *apThis, ActivePhysics *apOther);

	bool collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther);

	bool collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther);

};


void daFakeStarCoin::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther) { 

	PlaySound(this, SE_EMY_CS_TERESA_BEAT_YOU);
	SpawnEffect("Wm_en_obakedoor", 0, &this->pos, &(S16Vec){0,0,0}, &(Vec){1.0, 1.0, 1.0});
	
	//FIXME changed to dStageActor_c::Delete(u8) from fBase_c::Delete(void)
	this->Delete(1);
}

bool daFakeStarCoin::collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther) {
	return collisionCat9_RollingObject(apThis, apOther);
}
bool daFakeStarCoin::collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther) {
	return collisionCat9_RollingObject(apThis, apOther);
}
bool daFakeStarCoin::collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther) {
	return collisionCat9_RollingObject(apThis, apOther);
}
bool daFakeStarCoin::collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther) {
	return collisionCat9_RollingObject(apThis, apOther);
}

bool daFakeStarCoin::collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther) { 
	SpawnEffect("Wm_en_explosion", 0, &this->pos, &(S16Vec){0,0,0}, &(Vec){1.0, 1.0, 1.0});
	SpawnEffect("Wm_en_explosion_smk", 0, &this->pos, &(S16Vec){0,0,0}, &(Vec){3.0, 3.0, 3.0});

	PlaySound(this, SE_OBJ_EMY_FIRE_DISAPP);
	this->Delete(1);

	return true;
}
bool daFakeStarCoin::collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther) { 
	SpawnEffect("Wm_ob_cmnicekira", 0, &this->pos, &(S16Vec){0,0,0}, &(Vec){1.5, 1.5, 1.5});
	SpawnEffect("Wm_ob_icebreakwt", 0, &this->pos, &(S16Vec){0,0,0}, &(Vec){1.0, 1.0, 1.0});
	SpawnEffect("Wm_ob_iceattack", 0, &this->pos, &(S16Vec){0,0,0}, &(Vec){1.5, 1.5, 1.5});

	PlaySound(this, SE_OBJ_PNGN_ICE_BREAK);

	this->Delete(1);
	return true; 
}
bool daFakeStarCoin::collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther) {
	SpawnEffect("Wm_ob_cmnboxgrain", 0, &this->pos, &(S16Vec){0,0,0}, &(Vec){1.0, 1.0, 1.0});
	SpawnEffect("Wm_en_obakedoor_sm", 0, &this->pos, &(S16Vec){0,0,0}, &(Vec){1.0, 1.0, 1.0});

	PlaySound(this, SE_BOSS_JR_FLOOR_BREAK);

	//FIXME changed to dStageActor_c::Delete(u8) from fBase_c::Delete(void)
	this->Delete(1);
	return true;
}
bool daFakeStarCoin::collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther) {
	return collisionCat9_RollingObject(apThis, apOther);
}
bool daFakeStarCoin::collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther) {
	return collisionCat1_Fireball_E_Explosion(apThis, apOther);
}

bool daFakeStarCoin::collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther) {
	return collisionCat9_RollingObject(apThis, apOther);
}



daFakeStarCoin *daFakeStarCoin::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daFakeStarCoin));
	return new(buffer) daFakeStarCoin;
}


int daFakeStarCoin::onCreate() {
	
	allocator.link(-1, GameHeaps[0], 0, 0x20);

	nw4r::g3d::ResFile rf(getResource("star_coin", "g3d/star_coin.brres"));
	bodyModel.setup(rf.GetResMdl("star_coinA"), &allocator, 0x224, 1, 0);
	SetupTextures_MapObj(&bodyModel, 0);

	allocator.unlink();
	
	ActivePhysics::Info HitMeBaby;
	HitMeBaby.xDistToCenter = 0.0;
	HitMeBaby.yDistToCenter = -3.0;
	HitMeBaby.xDistToEdge = 12.0;
	HitMeBaby.yDistToEdge = 15.0;
	HitMeBaby.category1 = 0x5;
	HitMeBaby.category2 = 0x0;
	HitMeBaby.bitfield1 = 0x4F;
	HitMeBaby.bitfield2 = 0xFFFFFFFF;
	HitMeBaby.unkShort1C = 0;
	HitMeBaby.callback = &dEn_c::collisionCallback;

	this->aPhysics.initWithStruct(this, &HitMeBaby);
	this->aPhysics.addToList();

	this->scale.x = 1.0;
	this->scale.y = 1.0;
	this->scale.z = 1.0;

	this->pos.x -= 120.0;
	this->pos.z = 3300.0;
	
	this->onExecute();
	return true;
}


int daFakeStarCoin::onDelete() {
	return true;
}

int daFakeStarCoin::onDraw() {
	bodyModel.scheduleForDrawing();
	return true;
}


void daFakeStarCoin::updateModelMatrices() {
	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	bodyModel.setDrawMatrix(matrix);
	bodyModel.setScale(&scale);
	bodyModel.calcWorld(false);
}

int daFakeStarCoin::onExecute() {
	updateModelMatrices();

	this->rot.x += 0x200;
	return true;
}


//
// processed\../src/shyguy.cpp
//

#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>


const char* SGarcNameList [] = {
	"shyguy",
	"iron_ball",
	NULL	
};

// Shy Guy Settings
// 
// Nybble 5: Shy Guy Types
//		0 - Walker 	
//		1 - Pacing Walker
//		2 - Sleeper
//		3 - Jumper
// 		4 - Judo Master 
// 		5 - Spike Thrower
// 		6 - Ballooneer Horizontal
// 		7 - Ballooneer Vertical
// 		8 - Ballooneer Circular 
//		9 - Walking Giant
// 		10 - Pacing Giant
//
// Nybble 9: Distance Moved
//		# - Distance for Pacing Walker, Pacing Giants, and Ballooneers 
//
// If I add items in the balloons....
// I_kinoko, I_fireflower, I_propeller_model, I_iceflower, I_star, I_penguin - model names
// anmChr - wait2

void shyCollisionCallback(ActivePhysics *apThis, ActivePhysics *apOther);	
void ChucklesAndKnuckles(ActivePhysics *apThis, ActivePhysics *apOther);
void balloonSmack(ActivePhysics *apThis, ActivePhysics *apOther);

class daShyGuy : public dEn_c {
	int onCreate();
	int onDelete();
	int onExecute();
	int onDraw();

	mHeapAllocator_c allocator;
	nw4r::g3d::ResFile resFile;
	nw4r::g3d::ResFile anmFile;
	nw4r::g3d::ResFile balloonFile;
	// nw4r::g3d::ResFile carryFile;

	m3d::mdl_c bodyModel;
	m3d::mdl_c balloonModel;
	m3d::mdl_c balloonModelB;
	// m3d::mdl_c carryModel;1

	m3d::anmChr_c chrAnimation;
	// m3d::anmChr_c carryAnm;

	mEf::es2 effect;

	int timer;
	int jumpCounter;
	int baln;
	float dying;
	float Baseline;
	char damage;
	char isDown;
	char renderBalloon;
	Vec initialPos;
	int distance;
	float XSpeed;
	u32 cmgr_returnValue;
	bool isBouncing;
	float balloonSize;
	char backFire;
	char spikeTurn;
	int directionStore;
	dStageActor_c *spikeA;
	dStageActor_c *spikeB;
	bool stillFalling;

	StandOnTopCollider giantRider;
	ActivePhysics Chuckles;
	ActivePhysics Knuckles;
	ActivePhysics balloonPhysics;

	static daShyGuy *build();

	void bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate);
	void updateModelMatrices();
	bool calculateTileCollisions();

	void spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	void playerCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	void yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther);

	bool collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther);
	// bool collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther);

	bool collisionCat5_Mario(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat11_PipeCannon(ActivePhysics *apThis, ActivePhysics *apOther);

	void _vf148();
	void _vf14C();
	bool CreateIceActors();

	bool willWalkOntoSuitableGround();

	USING_STATES(daShyGuy);
	DECLARE_STATE(Walk);
	DECLARE_STATE(Turn);
	DECLARE_STATE(RealWalk);
	DECLARE_STATE(RealTurn);
	DECLARE_STATE(Jump);
	DECLARE_STATE(Sleep);
	DECLARE_STATE(Balloon_H);
	DECLARE_STATE(Balloon_V);
	DECLARE_STATE(Balloon_C);
	DECLARE_STATE(Judo);
	DECLARE_STATE(Spike);

	DECLARE_STATE(GoDizzy);
	DECLARE_STATE(BalloonDrop);
	DECLARE_STATE(FireKnockBack);
	DECLARE_STATE(FlameHit);
	DECLARE_STATE(Recover);

	DECLARE_STATE(Die);

	public: void popBalloon();
	int type;
};

daShyGuy *daShyGuy::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daShyGuy));
	return new(buffer) daShyGuy;
}

///////////////////////
// Externs and States
///////////////////////
	extern "C" bool SpawnEffect(const char*, int, Vec*, S16Vec*, Vec*);

	//FIXME make this dEn_c->used...
	extern "C" char usedForDeterminingStatePress_or_playerCollision(dEn_c* t, ActivePhysics *apThis, ActivePhysics *apOther, int unk1);
	extern "C" int SomeStrangeModification(dStageActor_c* actor);
	extern "C" void DoStuffAndMarkDead(dStageActor_c *actor, Vec vector, float unk);
	extern "C" int SmoothRotation(short* rot, u16 amt, int unk2);
	// extern "C" void addToList(StandOnTopCollider *self);

	extern "C" bool HandlesEdgeTurns(dEn_c* actor);


	CREATE_STATE(daShyGuy, Walk);
	CREATE_STATE(daShyGuy, Turn);
	CREATE_STATE(daShyGuy, RealWalk);
	CREATE_STATE(daShyGuy, RealTurn);
	CREATE_STATE(daShyGuy, Jump);
	CREATE_STATE(daShyGuy, Sleep);
	CREATE_STATE(daShyGuy, Balloon_H);
	CREATE_STATE(daShyGuy, Balloon_V);
	CREATE_STATE(daShyGuy, Balloon_C);
	CREATE_STATE(daShyGuy, Judo);
	CREATE_STATE(daShyGuy, Spike);

	CREATE_STATE(daShyGuy, GoDizzy);
	CREATE_STATE(daShyGuy, BalloonDrop);
	CREATE_STATE(daShyGuy, FireKnockBack);
	CREATE_STATE(daShyGuy, FlameHit);
	CREATE_STATE(daShyGuy, Recover);

	CREATE_STATE(daShyGuy, Die);

////////////////////////
// Collision Functions
////////////////////////

	bool actorCanPopBalloon(dStageActor_c *ac) {
		int n = ac->name;
		return n == PLAYER || n == YOSHI ||
			n == PL_FIREBALL || n == ICEBALL ||
			n == YOSHI_FIRE || n == HAMMER;
	}
	// Collision callback to help shy guy not die at inappropriate times and ruin the dinner

	void shyCollisionCallback(ActivePhysics *apThis, ActivePhysics *apOther) {
		int t = ((daShyGuy*)apThis->owner)->type;
		if (t == 6 || t == 7 || t == 8) {
			// Should I do something about ice blocks here?
			if (actorCanPopBalloon(apOther->owner))
				((daShyGuy*)apThis->owner)->popBalloon();
		}

		if ((apOther->owner->name == 89) && (t == 5)) { return; }
			
		dEn_c::collisionCallback(apThis, apOther); 
	}

	void ChucklesAndKnuckles(ActivePhysics *apThis, ActivePhysics *apOther) {
		if (apOther->owner->name != PLAYER) { return; }
		((dEn_c*)apThis->owner)->_vf220(apOther->owner);
	}

	void balloonSmack(ActivePhysics *apThis, ActivePhysics *apOther) {
		if (((daShyGuy*)apThis->owner)->frzMgr._mstate == 0) {
			if (actorCanPopBalloon(apOther->owner))
				((daShyGuy*)apThis->owner)->popBalloon();
		}
	}

	void daShyGuy::spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
		u16 name = ((dEn_c*)apOther->owner)->name;

		if (name == EN_COIN || name == EN_EATCOIN || name == AC_BLOCK_COIN || name == EN_COIN_JUGEM || name == EN_COIN_ANGLE
			|| name == EN_COIN_JUMP || name == EN_COIN_FLOOR || name == EN_COIN_VOLT || name == EN_COIN_WIND 
			|| name == EN_BLUE_COIN || name == EN_COIN_WATER || name == EN_REDCOIN || name == EN_GREENCOIN
			|| name == EN_JUMPDAI || name == EN_ITEM) 
			{ return; }

		if (acState.getCurrentState() == &StateID_RealWalk) {

			pos.x = ((pos.x - ((dEn_c*)apOther->owner)->pos.x) > 0) ? pos.x + 1.5 : pos.x - 1.5;
			// pos.x = direction ? pos.x + 1.5 : pos.x - 1.5;
			doStateChange(&StateID_RealTurn); }

		if (acState.getCurrentState() == &StateID_FireKnockBack) {
			float distance = pos.x - ((dEn_c*)apOther->owner)->pos.x;
			pos.x = pos.x + (distance / 4.0);
		}

		dEn_c::spriteCollision(apThis, apOther); 
	}

	void daShyGuy::popBalloon() {
		doStateChange(&StateID_BalloonDrop);
	}

	void daShyGuy::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
		dStateBase_c *stateVar;
		dStateBase_c *deathState;
		
		char hitType;
		if (this->type < 6) {  // Regular Shy Guys
			stateVar = &StateID_GoDizzy;
			deathState = &StateID_Die;
		}
		else { // Ballooneers
			stateVar = &StateID_BalloonDrop;
			deathState = &StateID_Die;
		}


		if (this->isDown == 0) { 
			hitType = usedForDeterminingStatePress_or_playerCollision(this, apThis, apOther, 2);
		}
		else { // Shy Guy is in downed mode
			hitType = usedForDeterminingStatePress_or_playerCollision(this, apThis, apOther, 0);
		}

		if(hitType == 1) {	// regular jump
			apOther->someFlagByte |= 2;
			if (this->isDown == 0) { 
				this->playEnemyDownSound1();
				if (damage >= 1) {
					doStateChange(deathState); }
				else {
					doStateChange(stateVar); }
				damage++;
			}
			else { // Shy Guy is in downed mode - kill it with fire
				this->playEnemyDownSound1();
				doStateChange(deathState);
			}				
		} 
		else if(hitType == 3) {	// spinning jump or whatever?
			apOther->someFlagByte |= 2;
			if (this->isDown == 0) { 
				this->playEnemyDownSound1();
				if (damage >= 1) {
					doStateChange(deathState); }
				else {
					doStateChange(stateVar); }
				damage++;
			}
			else { // Shy Guy is in downed mode - kill it with fire
				this->playEnemyDownSound1();
				doStateChange(deathState);
			}				
		} 
		else if(hitType == 0) {
			this->dEn_c::playerCollision(apThis, apOther);
			this->_vf220(apOther->owner);
		} 
		// else if(hitType == 2) { \\ Minimario? } 
	}

	void daShyGuy::yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
		this->playerCollision(apThis, apOther);
	}
	bool daShyGuy::collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther) { 
		PlaySound(this, SE_EMY_DOWN); 
		SpawnEffect("Wm_mr_hardhit", 0, &pos, &(S16Vec){0,0,0}, &(Vec){1.0, 1.0, 1.0});
		//addScoreWhenHit accepts a player parameter.
		//DON'T DO THIS:
		// this->addScoreWhenHit(this);
		doStateChange(&StateID_Die); 
		return true;
	}
	bool daShyGuy::collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther) {
		return this->collisionCatD_Drill(apThis, apOther);
	}
	bool daShyGuy::collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther) {
		return this->collisionCatD_Drill(apThis, apOther);
	}
	bool daShyGuy::collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther) {
		return this->collisionCatD_Drill(apThis, apOther);
	}
	bool daShyGuy::collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther){
		return this->collisionCatD_Drill(apThis, apOther);
	}
	bool daShyGuy::collisionCat5_Mario(ActivePhysics *apThis, ActivePhysics *apOther){
		return this->collisionCatD_Drill(apThis, apOther);
	}
	bool daShyGuy::collisionCat11_PipeCannon(ActivePhysics *apThis, ActivePhysics *apOther){
		return this->collisionCatD_Drill(apThis, apOther);
	}
	bool daShyGuy::collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther) {
		StageE4::instance->spawnCoinJump(pos, 0, 2, 0);
		return this->collisionCatD_Drill(apThis, apOther);
	}

	bool daShyGuy::collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther){
		bool wut = dEn_c::collisionCat3_StarPower(apThis, apOther);
		doStateChange(&StateID_Die);
		return wut;
	}

	bool daShyGuy::collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther){
		doStateChange(&StateID_DieSmoke);
		return true;
	}
	bool daShyGuy::collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther) {
		this->damage += 1;

		dStateBase_c *stateVar;
		stateVar = &StateID_DieSmoke;
		
		if (this->type < 6) {  // Regular Shy Guys Except Jumper

			backFire = apOther->owner->direction ^ 1;
			
			// if (this->isDown == 0) {
			// 	stateVar = &StateID_FireKnockBack;
			// }
			// else {
				StageE4::instance->spawnCoinJump(pos, 0, 1, 0);
				doStateChange(&StateID_DieSmoke);
			// }
		}
		else { // Ballooneers
			stateVar = &StateID_FlameHit;
		}

		if (this->damage > 1) {
			nw4r::snd::SoundHandle handle;
			PlaySoundWithFunctionB4(SoundRelatedClass, &handle, SE_EMY_DOWN, 1);
			StageE4::instance->spawnCoinJump(pos, 0, 1, 0);
			doStateChange(&StateID_DieSmoke);
		}
		else {
			nw4r::snd::SoundHandle handle;
			PlaySoundWithFunctionB4(SoundRelatedClass, &handle, SE_EMY_KURIBO_L_DAMAGE_01, 1);
			doStateChange(stateVar);
		}
		return true;
	}

	// void daShyGuy::collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther) {
		
	// 	dEn_C::collisionCat2_IceBall_15_YoshiIce(apThis, apOther);
	// }

	// These handle the ice crap
	void daShyGuy::_vf148() {
		dEn_c::_vf148();
		doStateChange(&StateID_Die);
	}
	void daShyGuy::_vf14C() {
		dEn_c::_vf14C();
		doStateChange(&StateID_Die);
	}

	extern "C" void sub_80024C20(void);
	extern "C" void __destroy_arr(void*, void(*)(void), int, int);
	//extern "C" __destroy_arr(struct DoSomethingCool, void(*)(void), int cnt, int bar);

	bool daShyGuy::CreateIceActors() {
		struct DoSomethingCool my_struct = { 0, this->pos, {1.2, 1.5, 1.5}, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
	    this->frzMgr.Create_ICEACTORs( (void*)&my_struct, 1 );
	    __destroy_arr( (void*)&my_struct, sub_80024C20, 0x3C, 1 );
	    chrAnimation.setUpdateRate(0.0f);
	    return true;
	}

bool daShyGuy::calculateTileCollisions() {
	// Returns true if sprite should turn, false if not.

	HandleXSpeed();
	HandleYSpeed();
	doSpriteMovement();

	cmgr_returnValue = collMgr.isOnTopOfTile();
	collMgr.calculateBelowCollisionWithSmokeEffect();

	if (isBouncing) {
		stuffRelatingToCollisions(0.1875f, 1.0f, 0.5f);
		if (speed.y != 0.0f)
			isBouncing = false;
	}

	float xDelta = pos.x - last_pos.x;
	if (xDelta >= 0.0f)
		direction = 0;
	else
		direction = 1;

	if (collMgr.isOnTopOfTile()) {
		// Walking into a tile branch

		if (cmgr_returnValue == 0)
			isBouncing = true;

		if (speed.x != 0.0f) {
			//playWmEnIronEffect();
		}

		speed.y = 0.0f;

		// u32 blah = collMgr.s_80070760();
		// u8 one = (blah & 0xFF);
		// static const float incs[5] = {0.00390625f, 0.0078125f, 0.015625f, 0.0234375f, 0.03125f};
		// x_speed_inc = incs[one];
		max_speed.x = (direction == 1) ? -XSpeed : XSpeed;
	} else {
		x_speed_inc = 0.0f;
	}

	// Bouncing checks
	if (_34A & 4) {
		Vec v = (Vec){0.0f, 1.0f, 0.0f};
		collMgr.pSpeed = &v;

		if (collMgr.calculateAboveCollision(collMgr.outputMaybe))
			speed.y = 0.0f;

		collMgr.pSpeed = &speed;

	} else {
		if (collMgr.calculateAboveCollision(collMgr.outputMaybe))
			speed.y = 0.0f;
	}

	collMgr.calculateAdjacentCollision(0);

	// Switch Direction
	if (collMgr.outputMaybe & (0x15 << direction)) {
		if (collMgr.isOnTopOfTile()) {
			isBouncing = true;
		}
		return true;
	}
	return false;
}

void daShyGuy::bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate) {
	nw4r::g3d::ResAnmChr anmChr = this->anmFile.GetResAnmChr(name);
	this->chrAnimation.bind(&this->bodyModel, anmChr, unk);
	this->bodyModel.bindAnim(&this->chrAnimation, unk2);
	this->chrAnimation.setUpdateRate(rate);
}

int daShyGuy::onCreate() {

	this->type = this->settings >> 28 & 0xF;
	int baln = this->settings >> 24 & 0xF;
	this->distance = this->settings >> 12 & 0xF;

	stillFalling = 0;

	allocator.link(-1, GameHeaps[0], 0, 0x20);

	this->deleteForever = 1;

	// Balloon Specifics
	if (type == 6 || type == 7 || type == 8) {
		this->renderBalloon = 1;

		this->balloonFile.data = getResource("shyguy", "g3d/balloon.brres");
		nw4r::g3d::ResMdl mdlB = this->balloonFile.GetResMdl("ballon");
		balloonModel.setup(mdlB, &allocator, 0x224, 1, 0);
		balloonModelB.setup(mdlB, &allocator, 0x224, 1, 0);

		ActivePhysics::Info iballoonPhysics;

		iballoonPhysics.xDistToCenter = 0.0;
		iballoonPhysics.yDistToCenter = -18.0;
		iballoonPhysics.xDistToEdge   = 13.0;
		iballoonPhysics.yDistToEdge   = 12.0;

		iballoonPhysics.category1  = 0x3;
		iballoonPhysics.category2  = 0x0;
		iballoonPhysics.bitfield1  = 0x4f;
		iballoonPhysics.bitfield2  = 0xffbafffe;
		iballoonPhysics.unkShort1C = 0x0;
		iballoonPhysics.callback   = balloonSmack;

		balloonPhysics.initWithStruct(this, &iballoonPhysics);
		balloonPhysics.addToList();


		// if (baln != 0) {
		// 	char *itemArc;
		// 	char *itemBrres;
		// 	char *itemMdl;

		// 	if (baln == 1) { 
		// 		itemArc		= "I_kinoko";
		// 		itemBrres	= "g3d/I_kinoko.brres";
		// 		itemMdl		= "I_kinoko";
		// 	}
		// 	else if (baln == 2) { 
		// 		itemArc		= "I_fireflower";
		// 		itemBrres	= "g3d/I_fireflower.brres";
		// 		itemMdl		= "I_fireflower";
		// 	}
		// 	else if (baln == 3) { 
		// 		itemArc		= "I_propeller";
		// 		itemBrres	= "g3d/I_propeller.brres";
		// 		itemMdl		= "I_propeller_model";
		// 	}
		// 	else if (baln == 4) { 
		// 		itemArc		= "I_iceflower";
		// 		itemBrres	= "g3d/I_iceflower.brres";
		// 		itemMdl		= "I_iceflower";
		// 	}
		// 	else if (baln == 5) { 
		// 		itemArc		= "I_star";
		// 		itemBrres	= "g3d/I_star.brres";
		// 		itemMdl		= "I_star";
		// 	}
		// 	else if (baln == 6) { 
		// 		itemArc		= "I_penguin";
		// 		itemBrres	= "g3d/I_penguin.brres";
		// 		itemMdl		= "I_penguin";
		// 	}

		// 	this->carryFile.data = getResource(itemArc, itemBrres);
		// 	nw4r::g3d::ResMdl mdlC = this->carryFile.GetResMdl(itemMdl);
		// 	carryModel.setup(mdlC, &allocator, 0x224, 1, 0);

		// 	nw4r::g3d::ResAnmChr anmChrC = this->carryFile.GetResAnmChr("wait2");
		// 	this->carryAnm.setup(mdlC, anmChrC, &this->allocator, 0);

		// 	this->carryAnm.bind(&this->carryModel, anmChrC, 1);
		// 	this->carryModel.bindAnim(&this->carryAnm, 0.0);
		// 	this->carryAnm.setUpdateRate(1.0);
		// }
	}
	else {this->renderBalloon = 0;}


	// Shy Guy Colours
	if (type == 1 || type == 8 || type == 10) {
		this->resFile.data = getResource("shyguy", "g3d/ShyGuyBlue.brres");
		distance = 1;
	}
	else if (type == 5) {
		this->resFile.data = getResource("shyguy", "g3d/ShyGuyGreen.brres");
	}
	else if (type == 3) {
		this->resFile.data = getResource("shyguy", "g3d/ShyGuyCyan.brres");
	}
	else if (type == 4) {
		this->resFile.data = getResource("shyguy", "g3d/ShyGuyPurple.brres");
	}
	else {
		this->resFile.data = getResource("shyguy", "g3d/ShyGuyRed.brres");
	}
	nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("body_h");
	bodyModel.setup(mdl, &allocator, 0x224, 1, 0);


	// Animations start here
	this->anmFile.data = getResource("shyguy", "g3d/ShyGuyAnimations.brres");
	nw4r::g3d::ResAnmChr anmChr = this->anmFile.GetResAnmChr("c18_IDLE_R");
	this->chrAnimation.setup(mdl, anmChr, &this->allocator, 0);

	allocator.unlink();

	// Stuff I do understand

	this->scale = (Vec){20.0, 20.0, 20.0};

	this->pos.y += 36.0;
	this->rot.x = 0; // X is vertical axis
	this->rot.y = 0xD800; // Y is horizontal axis
	this->rot.z = 0; // Z is ... an axis >.>
	this->direction = 1; // Heading left.
	
	this->speed.x = 0.0;
	this->speed.y = 0.0;
	this->max_speed.x = 0.6;
	this->x_speed_inc = 0.15;
	this->Baseline = this->pos.y;
	this->XSpeed = 0.6;
	this->balloonSize = 1.5;


	ActivePhysics::Info HitMeBaby;

	// Note: if this gets changed, also change the point where the default
	// values are assigned after de-ballooning
	HitMeBaby.xDistToCenter = 0.0;
	HitMeBaby.yDistToCenter = 12.0;
	HitMeBaby.xDistToEdge = 8.0;
	HitMeBaby.yDistToEdge = 12.0;
	if (renderBalloon) {
		HitMeBaby.yDistToCenter = 9.0f;
		HitMeBaby.yDistToEdge = 9.0f;
	}

	HitMeBaby.category1 = 0x3;
	HitMeBaby.category2 = 0x0;
	HitMeBaby.bitfield1 = 0x6F;
	HitMeBaby.bitfield2 = 0xffbafffe;
	HitMeBaby.unkShort1C = 0;
	HitMeBaby.callback = &shyCollisionCallback;

	this->aPhysics.initWithStruct(this, &HitMeBaby);
	this->aPhysics.addToList();


	// Tile collider

	// These fucking rects do something for the tile rect
	spriteSomeRectX = 28.0f;
	spriteSomeRectY = 32.0f;
	_320 = 0.0f;
	_324 = 16.0f;

	// These structs tell stupid collider what to collide with - these are from koopa troopa
	static const lineSensor_s below(-5<<12, 5<<12, 0<<12);
	static const pointSensor_s above(0<<12, 12<<12);
	static const lineSensor_s adjacent(6<<12, 9<<12, 6<<12);

	collMgr.init(this, &below, &above, &adjacent);
	collMgr.calculateBelowCollisionWithSmokeEffect();

	cmgr_returnValue = collMgr.isOnTopOfTile();

	if (collMgr.isOnTopOfTile())
		isBouncing = false;
	else
		isBouncing = true;


	// State Changers
	
	if (type == 0) {
		bindAnimChr_and_setUpdateRate("c18_EV_WIN_2_R", 1, 0.0, 1.5); 
		doStateChange(&StateID_RealWalk);
	}		
	else if (type == 1) {
		bindAnimChr_and_setUpdateRate("c18_EV_WIN_2_R", 1, 0.0, 1.5); 
		doStateChange(&StateID_RealWalk);
	}		
	else if (type == 2) {
		doStateChange(&StateID_Sleep);
	}		
	else if (type == 3) {
		doStateChange(&StateID_Jump);
	}		
	else if (type == 4) {
		// Chuckles is left, Knuckles is Right
		ActivePhysics::Info iChuckles;
		ActivePhysics::Info iKnuckles;

		iChuckles.xDistToCenter = -27.0;
		iChuckles.yDistToCenter = 12.0;
		iChuckles.xDistToEdge   = 27.0;
		iChuckles.yDistToEdge   = 10.0;

		iKnuckles.xDistToCenter = 27.0;
		iKnuckles.yDistToCenter = 12.0;
		iKnuckles.xDistToEdge   = 27.0;
		iKnuckles.yDistToEdge   = 10.0;

		iKnuckles.category1  = iChuckles.category1  = 0x3;
		iKnuckles.category2  = iChuckles.category2  = 0x0;
		iKnuckles.bitfield1  = iChuckles.bitfield1  = 0x4F;
		iKnuckles.bitfield2  = iChuckles.bitfield2  = 0x0;
		iKnuckles.unkShort1C = iChuckles.unkShort1C = 0x0;
		iKnuckles.callback   = iChuckles.callback   = ChucklesAndKnuckles;

		Chuckles.initWithStruct(this, &iChuckles);
		Knuckles.initWithStruct(this, &iKnuckles);

		doStateChange(&StateID_Judo);
	}		
	else if (type == 5) {
		doStateChange(&StateID_Spike);
	}		
	else if (type == 6) {
		doStateChange(&StateID_Balloon_H);
	}		
	else if (type == 7) {
		doStateChange(&StateID_Balloon_V);
	}		
	else if (type == 8) {
		doStateChange(&StateID_Balloon_C);
	}		

	this->onExecute();
	return true;
}

int daShyGuy::onDelete() {
	return true;
}

int daShyGuy::onExecute() {
	acState.execute();
	updateModelMatrices();
	bodyModel._vf1C();

	return true;
}

int daShyGuy::onDraw() {
	bodyModel.scheduleForDrawing();

	if (this->renderBalloon == 1) {
		balloonModel.scheduleForDrawing();
		balloonModelB.scheduleForDrawing();
	}

	// if (this->baln > 0) {
	// 	carryModel.scheduleForDrawing();
	// 	carryModel._vf1C();

	// 	if(this->carryAnm.isAnimationDone())
	// 		this->carryAnm.setCurrentFrame(0.0);
	// }

	return true;
}

void daShyGuy::updateModelMatrices() {
	// This won't work with wrap because I'm lazy.

	if (this->frzMgr._mstate == 1)
		matrix.translation(pos.x, pos.y, pos.z);
	else
		matrix.translation(pos.x, pos.y - 2.0, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	bodyModel.setDrawMatrix(matrix);
	bodyModel.setScale(&scale);
	bodyModel.calcWorld(false);

	if (this->renderBalloon == 1) {
		matrix.translation(pos.x, pos.y - 32.0, pos.z);

		balloonModel.setDrawMatrix(matrix);
		balloonModel.setScale(balloonSize, balloonSize, balloonSize);
		balloonModel.calcWorld(false);

		balloonModelB.setDrawMatrix(matrix);
		balloonModelB.setScale(balloonSize, balloonSize, balloonSize);
		balloonModelB.calcWorld(false);
	}

	// if (this->baln > 0) {
	// 	matrix.applyRotationYXZ(0,0,0);
	// 	matrix.translation(pos.x+40.0, pos.y - 28.0, pos.z + 1000.0);

	// 	carryModel.setDrawMatrix(matrix);
	// 	carryModel.setScale(21.0, 21.0, 21.0);
	// 	carryModel.calcWorld(false);
	// }

}

///////////////
// Walk State
///////////////
	void daShyGuy::beginState_Walk() { 
		this->timer = 0;
		this->rot.y = (direction) ? 0xD800 : 0x2800;

		this->max_speed.x = 0.0;
		this->speed.x = 0.0;
		this->x_speed_inc = 0.0;
	}
	void daShyGuy::executeState_Walk() { 
		chrAnimation.setUpdateRate(1.5f);

		this->pos.x += (direction) ? -0.4 : 0.4;

		if (this->timer > (this->distance * 32)) {
			doStateChange(&StateID_Turn);
		}

		if(this->chrAnimation.isAnimationDone())
			this->chrAnimation.setCurrentFrame(0.0);

		this->timer = this->timer + 1;
	}
	void daShyGuy::endState_Walk() { 
	}

///////////////
// Turn State
///////////////
	void daShyGuy::beginState_Turn() { 
		// bindAnimChr_and_setUpdateRate("c18_IDLE_R", 1, 0.0, 1.0);
		this->direction ^= 1;
		this->speed.x = 0.0;
	}
	void daShyGuy::executeState_Turn() { 

		if(this->chrAnimation.isAnimationDone())
			this->chrAnimation.setCurrentFrame(0.0);

		u16 amt = (this->direction == 0) ? 0x2800 : 0xD800;
		int done = SmoothRotation(&this->rot.y, amt, 0x800);

		if(done) {
			this->doStateChange(&StateID_Walk);
		}
	}
	void daShyGuy::endState_Turn() { 
	}

///////////////
// Jump State
///////////////
	void daShyGuy::beginState_Jump() { 
		this->max_speed.x = 0.0;
		this->speed.x = 0.0;
		this->x_speed_inc = 0.0;

		this->timer = 0;
		this->jumpCounter = 0;
	}
	void daShyGuy::executeState_Jump() { 

		// Always face Mario
		u8 facing = dSprite_c__getXDirectionOfFurthestPlayerRelativeToVEC3(this, this->pos);

		if (facing != this->direction) {
			this->direction = facing;
			this->rot.y = (direction) ? 0xD800 : 0x2800;
		}

		// Shy Guy is on ground
		if (this->pos.y < this->Baseline) {

			bindAnimChr_and_setUpdateRate("c18_IDLE_R", 1, 0.0, 1.0);
			
			this->timer = this->timer + 1;

			// Make him wait for 0.5 seconds
			if (this->timer > 30) {

				if(this->chrAnimation.isAnimationDone())
					this->chrAnimation.setCurrentFrame(0.0);

				this->speed.x = 0;
				this->speed.y = 0;	
			}

			// Then Jump!
			else { 
				if (this->jumpCounter == 3) { this->jumpCounter = 0; }

				this->pos.y = this->Baseline + 1;
				this->timer = 0;
				this->jumpCounter = this->jumpCounter + 1;


				if (this->jumpCounter == 3) {
					bindAnimChr_and_setUpdateRate("c18_NORMAL_STEAL_R", 1, 0.0, 1.0);
					this->speed.y = 8.0;
					PlaySoundAsync(this, SE_PLY_JUMPDAI_HIGH);
				}
				else {
					bindAnimChr_and_setUpdateRate("c18_EV_WIN_1_R", 1, 0.0, 1.0);
					this->speed.y = 6.0;
					PlaySoundAsync(this, SE_PLY_JUMPDAI);
				}

			}
		}

		// While he's jumping, it's time for gravity.
		else { 

			this->speed.y = this->speed.y - 0.15; 

			if (this->jumpCounter == 3) {
				if(this->chrAnimation.isAnimationDone())
					this->chrAnimation.setCurrentFrame(0.0);
			}
			else {
				if(this->chrAnimation.isAnimationDone())
					this->chrAnimation.setCurrentFrame(0.0);
			}
		}

		this->HandleXSpeed();
		this->HandleYSpeed();
		this->UpdateObjectPosBasedOnSpeedValuesReal();
	}
	void daShyGuy::endState_Jump() { 
	}

///////////////
// Sleep State
///////////////
	void daShyGuy::beginState_Sleep() { 
		bindAnimChr_and_setUpdateRate("c18_EV_LOSE_2_R", 1, 0.0, 1.0);
		this->rot.y = 0x0000;
	}
	void daShyGuy::executeState_Sleep() { 
		if(this->chrAnimation.isAnimationDone())
			this->chrAnimation.setCurrentFrame(0.0);
	}
	void daShyGuy::endState_Sleep() { 
	}

///////////////
// Balloon H State
///////////////
	void daShyGuy::beginState_Balloon_H() { 
		bindAnimChr_and_setUpdateRate("c18_L_DMG_F_3_R", 1, 0.0, 1.0);
		this->timer = 0;
		this->initialPos = this->pos;
		this->rot.x = 0xFE00;
		this->rot.y = 0;
	}
	void daShyGuy::executeState_Balloon_H() { 

		// Makes him bob up and down
		this->pos.y = this->initialPos.y + ( sin(this->timer * 3.14 / 60.0) * 6.0 );

		// Makes him move side to side
		this->pos.x = this->initialPos.x + ( sin(this->timer * 3.14 / 600.0) * (float)this->distance * 8.0);

		this->timer = this->timer + 1;

		if(this->chrAnimation.isAnimationDone())
			this->chrAnimation.setCurrentFrame(0.0);

	}
	void daShyGuy::endState_Balloon_H() { 
	}

///////////////
// Balloon V State
///////////////
	void daShyGuy::beginState_Balloon_V() { 
		bindAnimChr_and_setUpdateRate("c18_L_DMG_F_3_R", 1, 0.0, 1.0);
		this->timer = 0;
		this->initialPos = this->pos;
		this->rot.x = 0xFE00;
		this->rot.y = 0;
	}
	void daShyGuy::executeState_Balloon_V() { 
		// Makes him bob up and down
		this->pos.x = this->initialPos.x + ( sin(this->timer * 3.14 / 60.0) * 6.0 );

		// Makes him move side to side
		this->pos.y = this->initialPos.y + ( sin(this->timer * 3.14 / 600.0) * (float)this->distance * 8.0 );

		this->timer = this->timer + 1;

		if(this->chrAnimation.isAnimationDone())
			this->chrAnimation.setCurrentFrame(0.0);
	}
	void daShyGuy::endState_Balloon_V() { 
	}

///////////////
// Balloon C State
///////////////
	void daShyGuy::beginState_Balloon_C() { 
		bindAnimChr_and_setUpdateRate("c18_L_DMG_F_3_R", 1, 0.0, 1.0);
		this->timer = 0;
		this->initialPos = this->pos;
		this->rot.x = 0xFE00;
		this->rot.y = 0;
	}
	void daShyGuy::executeState_Balloon_C() { 
		// Makes him bob up and down
		this->pos.x = this->initialPos.x + ( sin(this->timer * 3.14 / 600.0) * (float)this->distance * 8.0 );

		// Makes him move side to side
		this->pos.y = this->initialPos.y + ( cos(this->timer * 3.14 / 600.0) * (float)this->distance * 8.0 );

		this->timer = this->timer + 1;

		if(this->chrAnimation.isAnimationDone())
			this->chrAnimation.setCurrentFrame(0.0);
	}
	void daShyGuy::endState_Balloon_C() { 
	}

///////////////
// Judo State
///////////////
	void daShyGuy::beginState_Judo() { 
		this->max_speed.x = 0.0;
		this->speed.x = 0.0;
		this->x_speed_inc = 0.0;
		this->pos.y -= 4.0;

		this->timer = 0;
	}
	void daShyGuy::executeState_Judo() { 

	// chargin 476? 673? 760? 768? 808? 966?
		if (this->timer == 0) { bindAnimChr_and_setUpdateRate("c18_OB_IDLE_R", 1, 0.0, 1.0); }

		this->timer = this->timer + 1;

		if (this->timer == 80) { 
			if (this->direction == 1) { 
				SpawnEffect("Wm_ob_keyget02_lighit", 0, &(Vec){this->pos.x + 7.0, this->pos.y + 14.0, this->pos.z - 5500.0}, &(S16Vec){0,0,0}, &(Vec){0.8, 0.8, 0.8});
			}
			else {
				SpawnEffect("Wm_ob_keyget02_lighit", 0, &(Vec){this->pos.x - 7.0, this->pos.y + 14.0, this->pos.z + 5500.0}, &(S16Vec){0,0,0}, &(Vec){0.8, 0.8, 0.8});
			}	
		}

		if (this->timer < 120) {
			// Always face Mario
			u8 facing = dSprite_c__getXDirectionOfFurthestPlayerRelativeToVEC3(this, this->pos);

			if (facing != this->direction) {
				this->direction = facing;
				if (this->direction == 1) {
					this->rot.y = 0xD800;
				}
				else {
					this->rot.y = 0x2800;
				}
			}


			if(this->chrAnimation.isAnimationDone())
				this->chrAnimation.setCurrentFrame(0.0);
		}

		else if (this->timer == 120) {
			bindAnimChr_and_setUpdateRate("c18_H_CUT_R", 1, 0.0, 1.0);
			
		}

		else if (this->timer == 132) {
			PlaySoundAsync(this, SE_EMY_CRASHER_PUNCH);

			if (this->direction == 1) { 
				SpawnEffect("Wm_mr_wallkick_b_l", 0, &(Vec){this->pos.x - 18.0, this->pos.y + 16.0, this->pos.z}, &(S16Vec){0,0,0}, &(Vec){1.5, 1.5, 1.5});
				Chuckles.addToList();
			}
			else {
				SpawnEffect("Wm_mr_wallkick_s_r", 0, &(Vec){this->pos.x + 18.0, this->pos.y + 16.0, this->pos.z}, &(S16Vec){0,0,0}, &(Vec){1.5, 1.5, 1.5});
				Knuckles.addToList();
			}	
		}

		else {

			if(this->chrAnimation.isAnimationDone()) {
				if (this->direction == 1) { 
					SpawnEffect("Wm_mr_wirehit_hit", 0, &(Vec){this->pos.x - 38.0, this->pos.y + 16.0, this->pos.z}, &(S16Vec){0,0,0}, &(Vec){1.0, 1.0, 1.0});
					Chuckles.removeFromList();
				}
				else {
					SpawnEffect("Wm_mr_wirehit_hit", 0, &(Vec){this->pos.x + 38.0, this->pos.y + 16.0, this->pos.z}, &(S16Vec){0,0,0}, &(Vec){1.0, 1.0, 1.0});
					Knuckles.removeFromList();	
				}

				this->timer = 0;
				PlaySoundAsync(this, SE_EMY_BIG_PAKKUN_DAMAGE_1);
			}
		}
	}
	void daShyGuy::endState_Judo() { 
	}

///////////////
// Spike State
///////////////
	void daShyGuy::beginState_Spike() { 
		this->timer = 80;
		spikeTurn = 0;

		this->max_speed.x = 0.0;
		this->speed.x = 0.0;
		this->x_speed_inc = 0.0;
		this->pos.y -= 4.0;
	}
	void daShyGuy::executeState_Spike() {

		if (this->timer == 0) { bindAnimChr_and_setUpdateRate("c18_OB_IDLE_R", 1, 0.0, 1.0); }

		if (this->timer < 120) {
			// Always face Mario
			u8 facing = dSprite_c__getXDirectionOfFurthestPlayerRelativeToVEC3(this, this->pos);

			if (facing != this->direction) {
				this->direction = facing;
				if (this->direction == 1) {
					this->rot.y = 0xD800;
				}
				else {
					this->rot.y = 0x2800;
				}
			}

			if(this->chrAnimation.isAnimationDone())
				this->chrAnimation.setCurrentFrame(0.0);
		}

		else if (this->timer == 120) {
			bindAnimChr_and_setUpdateRate("c18_H_SHOT_R", 1, 0.0, 1.0);			
		}

		else if (this->timer == 160) {
			PlaySound(this, SE_EMY_KANIBO_THROW);

			Vec pos;
			pos.x = this->pos.x;
			pos.y = this->pos.y;
			pos.z = this->pos.z;
			dStageActor_c *spawned;

			if (this->direction == 1) { 
				spawned = CreateActor(89, 0x2, pos, 0, 0);
				spawned->scale.x = 0.9;
				spawned->scale.y = 0.9;
				spawned->scale.z = 0.9;

				spawned->speed.x = -2.0;
				spawned->speed.y = 2.0;
			}
			else {
				spawned = CreateActor(89, 0x12, pos, 0, 0);
				spawned->scale.x = 0.9;
				spawned->scale.y = 0.9;
				spawned->scale.z = 0.9;

				spawned->speed.x = 2.0;
				spawned->speed.y = 2.0;
			}
		}

		else {

			if(this->chrAnimation.isAnimationDone()) {
				this->timer = 0;
				return;
			}
		}

		this->timer = this->timer + 1;

	}
	void daShyGuy::endState_Spike() { 
	}

///////////////
// Real Walk State
///////////////
bool daShyGuy::willWalkOntoSuitableGround() {
	static const float deltas[] = {2.5f, -2.5f};
	VEC3 checkWhere = {
			pos.x + deltas[direction],
			4.0f + pos.y,
			pos.z};

	u32 props = collMgr.getTileBehaviour2At(checkWhere.x, checkWhere.y, currentLayerID);

	//if (getSubType(props) == B_SUB_LEDGE)
	if (((props >> 16) & 0xFF) == 8)
		return false;

	float someFloat = 0.0f;
	if (collMgr.sub_800757B0(&checkWhere, &someFloat, currentLayerID, 1, -1)) {
		if (someFloat < checkWhere.y && someFloat > (pos.y - 5.0f))
			return true;
	}

	return false;
}


	void daShyGuy::beginState_RealWalk() {
		//inline this piece of code
		this->max_speed.x = (this->direction) ? -this->XSpeed : this->XSpeed;
		this->speed.x = (direction) ? -0.6f : 0.6f;

		this->max_speed.y = -4.0;
		this->speed.y = -4.0;
		this->y_speed_inc = -0.1875;
	}
	void daShyGuy::executeState_RealWalk() { 
		chrAnimation.setUpdateRate(1.5f);

		// if (distance) {
		// 	// What the fuck. Somehow, having this code makes the shyguy not
		// 	// fall through solid-on-top platforms...
		// 	bool turn = collMgr.isOnTopOfTile();
		// 	if (!turn) {
		// 		if (!stillFalling) {
		// 			stillFalling = true;
		// 			pos.x = direction ? pos.x + 1.5 : pos.x - 1.5;
		// 			doStateChange(&StateID_RealTurn);
		// 		}
		// 	} else 
		// }


		if (distance) {
			if (collMgr.isOnTopOfTile()) {
				stillFalling = false;

				if (!willWalkOntoSuitableGround()) {
					pos.x = direction ? pos.x + 1.5 : pos.x - 1.5;
					doStateChange(&StateID_RealTurn);
				}
			}
			else {
				if (!stillFalling) {
					stillFalling = true;
					pos.x = direction ? pos.x + 1.5 : pos.x - 1.5;
					doStateChange(&StateID_RealTurn);
				}
			}
		}

		bool ret = calculateTileCollisions();
		if (ret) {
			doStateChange(&StateID_RealTurn);
		}

		if(this->chrAnimation.isAnimationDone()) {
			this->chrAnimation.setCurrentFrame(0.0);
		}
	}
	void daShyGuy::endState_RealWalk() { }

///////////////
// Real Turn State
///////////////
	void daShyGuy::beginState_RealTurn() {

		this->direction ^= 1;
		this->speed.x = 0.0;
	}
	void daShyGuy::executeState_RealTurn() { 

		if(this->chrAnimation.isAnimationDone()) {
			this->chrAnimation.setCurrentFrame(0.0);
		}

		u16 amt = (this->direction == 0) ? 0x2800 : 0xD800;
		int done = SmoothRotation(&this->rot.y, amt, 0x800);

		if(done) {
			this->doStateChange(&StateID_RealWalk);
		}
	}
	void daShyGuy::endState_RealTurn() {
	}

///////////////
// GoDizzy State
///////////////
	void daShyGuy::beginState_GoDizzy() {
		bindAnimChr_and_setUpdateRate("c18_L_DMG_F_1_R", 1, 0.0, 1.0); 

		// SpawnEffect("Wm_en_spindamage", 0, &(Vec){this->pos.x, this->pos.y + 24.0, 0}, &(S16Vec){0,0,0}, &(Vec){1.0, 1.0, 1.0});

		this->max_speed.x = 0;
		this->speed.x = 0;
		this->x_speed_inc = 0;

		this->max_speed.y = -4.0;
		this->speed.y = -4.0;
		this->y_speed_inc = -0.1875;

		this->timer = 0;
		this->jumpCounter = 0;
		this->isDown = 1;
	}
	void daShyGuy::executeState_GoDizzy() { 
		calculateTileCollisions();
	
		effect.spawn("Wm_en_spindamage", 0, &(Vec){this->pos.x, this->pos.y + 24.0, 0}, &(S16Vec){0,0,0}, &(Vec){1.0, 1.0, 1.0});

		if (this->jumpCounter == 0) {
			if(this->chrAnimation.isAnimationDone()) {
				this->jumpCounter = 1;
				bindAnimChr_and_setUpdateRate("c18_L_DMG_F_3_R", 1, 0.0, 1.0); 
			}
		}

		else {
			if(this->chrAnimation.isAnimationDone()) {
				this->chrAnimation.setCurrentFrame(0.0);
			}

			if (this->timer > 600) {
				doStateChange(&StateID_Recover);
				damage = 0;
			}

			this->timer += 1;
		}
	}
	void daShyGuy::endState_GoDizzy() {}

///////////////
// BalloonDrop State
///////////////
	void daShyGuy::beginState_BalloonDrop() {
		bindAnimChr_and_setUpdateRate("c18_C_BLOCK_BREAK_R", 1, 0.0, 2.0); 

		this->max_speed.x = 0.0;
		this->speed.x = 0.0;
		this->x_speed_inc = 0.0;

		this->max_speed.y = -2.0;
		this->speed.y = -2.0;
		this->y_speed_inc = -0.1875;

		this->isDown = 1;
		this->renderBalloon = 0;

		// char powerup;

		// if (baln == 1) { 
		// 	powerup		= 0x0B000007;
		// }
		// else if (baln == 2) { 
		// 	powerup		= 0x0B000009;
		// }
		// else if (baln == 3) { 
		// 	powerup		= 0x0B000001;
		// }
		// else if (baln == 4) { 
		// 	powerup		= 0x0C00000E;
		// }
		// else if (baln == 5) { 
		// 	powerup		= 0x0C000015;
		// }
		// else if (baln == 6) { 
		// 	powerup		= 0x0C000011;
		// }
		// CreateActor(60, powerup, (Vec){pos.x, pos.y - 28.0, pos.z}, 0, 0);
		// this->baln = 0;

		balloonPhysics.removeFromList();
		SpawnEffect("Wm_en_explosion_ln", 0, &(Vec){this->pos.x, this->pos.y - 32.0, 0}, &(S16Vec){0,0,0}, &(Vec){1.0, 1.0, 1.0});
		PlaySound(this, SE_PLY_BALLOON_BRAKE); 

		if (this->type != 8)
			this-distance == 0;

		type = 0;
	}
	void daShyGuy::executeState_BalloonDrop() { 

		if(this->chrAnimation.isAnimationDone()) {
			this->chrAnimation.setCurrentFrame(0.0);
		}

		bool ret = calculateTileCollisions();

		if (speed.y == 0.0) { 
			SpawnEffect("Wm_en_sndlandsmk_s", 0, &(Vec){this->pos.x, this->pos.y, 0}, &(S16Vec){0,0,0}, &(Vec){1.0, 1.0, 1.0});
			doStateChange(&StateID_GoDizzy);

			aPhysics.info.yDistToCenter = 12.0f;
			aPhysics.info.yDistToEdge = 12.0f;
		}
	}
	void daShyGuy::endState_BalloonDrop() {
	}

///////////////
// FireKnockBack State
///////////////
	void daShyGuy::beginState_FireKnockBack() {
		bindAnimChr_and_setUpdateRate("c18_C_BLOCK_BREAK_R", 1, 0.0, 1.0); 

		// Backfire 0 == Fireball to the right
		// Backfire 1 == Fireball to the left

		directionStore = this->direction;
		speed.x = (this->backFire) ? this->XSpeed : -this->XSpeed;
		speed.x *= 1.2f;
		max_speed.x = speed.x;
		x_speed_inc = 0.0f;
	}
	void daShyGuy::executeState_FireKnockBack() { 

		calculateTileCollisions();
		// move backwards here
		this->speed.x = this->speed.x / 1.02f;

		if(this->chrAnimation.isAnimationDone()) {
			if (aPhysics.result1 == 0 && aPhysics.result2 == 0 && aPhysics.result3 == 0) {
				bindAnimChr_and_setUpdateRate("c18_EV_WIN_2_R", 1, 0.0, 1.5); 
				doStateChange(&StateID_RealWalk);
			}
		}
	}
	void daShyGuy::endState_FireKnockBack() {
		this->direction = directionStore;		
	}

///////////////
// FlameHit State
///////////////
	void daShyGuy::beginState_FlameHit() {
		bindAnimChr_and_setUpdateRate("c18_C_BLOCK_BREAK_R", 1, 0.0, 1.0); 
	}
	void daShyGuy::executeState_FlameHit() { 

		if(this->chrAnimation.isAnimationDone()) {
			if (type == 6) {
				doStateChange(&StateID_Balloon_H);
			}		
			else if (type == 7) {
				doStateChange(&StateID_Balloon_V);
			}		
			else if (type == 8) {
				doStateChange(&StateID_Balloon_C);
			}		
		}
	}
	void daShyGuy::endState_FlameHit() {}

///////////////
// Recover State
///////////////
	void daShyGuy::beginState_Recover() {
		bindAnimChr_and_setUpdateRate("c18_L_DMG_F_4_R", 1, 0.0, 1.0); 
	}
	void daShyGuy::executeState_Recover() { 

		calculateTileCollisions();

		if(this->chrAnimation.isAnimationDone()) {
			if (type == 3) {
				doStateChange(&StateID_Jump);
			}		
			else {
				bindAnimChr_and_setUpdateRate("c18_EV_WIN_2_R", 1, 0.0, 1.5); 
				doStateChange(&StateID_RealWalk);
			}
		}
	}
	void daShyGuy::endState_Recover() {
		this->isDown = 0;		
		this->rot.y = (direction) ? 0xD800 : 0x2800;
	}

///////////////
// Die State
///////////////
	void daShyGuy::beginState_Die() {
		// dEn_c::dieFall_Begin();
		this->removeMyActivePhysics();

		bindAnimChr_and_setUpdateRate("c18_C_BLOCK_BREAK_R", 1, 0.0, 2.0); 
		this->timer = 0;
		this->dying = -10.0;
		this->Baseline = this->pos.y;
		this->rot.y = 0;
		this->rot.x = 0;

		if (type > 5 && type < 9) {
			this->renderBalloon = 0;
			SpawnEffect("Wm_en_explosion_ln", 0, &(Vec){this->pos.x, this->pos.y - 32.0, 0}, &(S16Vec){0,0,0}, &(Vec){1.0, 1.0, 1.0});
		}
	}
	void daShyGuy::executeState_Die() { 

		if(this->chrAnimation.isAnimationDone()) {
			this->chrAnimation.setCurrentFrame(0.0);
		}

		this->timer += 1;
		 		
		// this->pos.x += 0.5; 
		this->pos.y = Baseline + (-0.2 * dying * dying) + 20.0;
		
		this->dying += 0.5;
			
		if (this->timer > 450) {
			OSReport("Killing");
			this->kill();
			this->Delete(this->deleteForever);
		}

		// dEn_c::dieFall_Execute();

	}
	void daShyGuy::endState_Die() {
	}


//
// processed\../src/shyguyGiants.cpp
//

#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>


const char* SGGarcNameList [] = {
	"shyguy",
	NULL	
};

// Shy Guy Settings
// 
// Nybble 5: Size
//		0 - Big 	
//		1 - Mega
//		2 - Giga
// 
// Nybble 6: Colour
//		0 - Red 
//		1 - Blue
//		2 - Green
//		3 - Cyan
//		4 - Magenta
//


class daShyGuyGiant : public dEn_c {
	int onCreate();
	int onDelete();
	int onExecute();
	int onDraw();

	mHeapAllocator_c allocator;
	nw4r::g3d::ResFile resFile;
	nw4r::g3d::ResFile anmFile;

	m3d::mdl_c bodyModel;
	m3d::anmChr_c chrAnimation;
	mEf::es2 effect;

	int timer;
	int type;
	float dying;
	float Baseline;
	char damage;
	char isDown;
	Vec initialPos;
	int distance;
	float XSpeed;
	u32 cmgr_returnValue;
	bool isBouncing;
	int directionStore;

	static daShyGuyGiant *build();

	void bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate);
	void updateModelMatrices();
	bool calculateTileCollisions();

	void playerCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	void yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther);

	bool collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat5_Mario(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat11_PipeCannon(ActivePhysics *apThis, ActivePhysics *apOther);

	void powBlockActivated(bool isNotMPGP);

	void _vf148();
	void _vf14C();
	bool CreateIceActors();
	void addScoreWhenHit(void *other);
	void bouncePlayerWhenJumpedOn(void *player);

	void spawnHitEffectAtPosition(Vec2 pos);
	void doSomethingWithHardHitAndSoftHitEffects(Vec pos);
	void playEnemyDownSound2();
	void playHpdpSound1(); // plays PLAYER_SE_EMY/GROUP_BOOT/SE_EMY_DOWN_HPDP_S or _H
	void playEnemyDownSound1();
	void playEnemyDownComboSound(void *player); // AcPy_c/daPlBase_c?
	void playHpdpSound2(); // plays PLAYER_SE_EMY/GROUP_BOOT/SE_EMY_DOWN_HPDP_S or _H
	void _vf260(void *other); // AcPy/PlBase? plays the SE_EMY_FUMU_%d sounds based on some value
	void _vf264(dStageActor_c *other); // if other is player or yoshi, do Wm_en_hit and a few other things
	void _vf268(void *other); // AcPy/PlBase? plays the SE_EMY_DOWN_SPIN_%d sounds based on some value
	void _vf278(void *other); // AcPy/PlBase? plays the SE_EMY_YOSHI_FUMU_%d sounds based on some value

	USING_STATES(daShyGuyGiant);
	DECLARE_STATE(RealWalk);
	DECLARE_STATE(RealTurn);
	DECLARE_STATE(Die);
};

daShyGuyGiant *daShyGuyGiant::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daShyGuyGiant));
	OSReport("Building Shy Guy");
	return new(buffer) daShyGuyGiant;
}

///////////////////////
// Externs and States
///////////////////////
	extern "C" bool SpawnEffect(const char*, int, Vec*, S16Vec*, Vec*);

	//FIXME make this dEn_c->used...
	extern "C" char usedForDeterminingStatePress_or_playerCollision(dEn_c* t, ActivePhysics *apThis, ActivePhysics *apOther, int unk1);
	extern "C" int SomeStrangeModification(dStageActor_c* actor);
	extern "C" void DoStuffAndMarkDead(dStageActor_c *actor, Vec vector, float unk);
	extern "C" int SmoothRotation(short* rot, u16 amt, int unk2);

	// Collision related
	extern "C" void BigHanaPlayer(dEn_c* t, ActivePhysics *apThis, ActivePhysics *apOther);
	extern "C" void BigHanaYoshi(dEn_c* t, ActivePhysics *apThis, ActivePhysics *apOther);
	extern "C" bool BigHanaWeirdGP(dEn_c* t, ActivePhysics *apThis, ActivePhysics *apOther);
	extern "C" bool BigHanaGroundPound(dEn_c* t, ActivePhysics *apThis, ActivePhysics *apOther);
	extern "C" bool BigHanaFireball(dEn_c* t, ActivePhysics *apThis, ActivePhysics *apOther);
	extern "C" bool BigHanaIceball(dEn_c* t, ActivePhysics *apThis, ActivePhysics *apOther);

	extern "C" void dAcPy_vf3F8(void* player, dEn_c* monster, int t);

	CREATE_STATE(daShyGuyGiant, RealWalk);
	CREATE_STATE(daShyGuyGiant, RealTurn);
	CREATE_STATE(daShyGuyGiant, Die);

////////////////////////
// Collision Functions
////////////////////////

	void daShyGuyGiant::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther) { 
		apOther->someFlagByte |= 2;

		dStageActor_c *player = apOther->owner;

		char hitType;
		hitType = usedForDeterminingStatePress_or_playerCollision(this, apThis, apOther, 0);
		if (hitType > 0) {
			PlaySound(this, SE_EMY_CMN_STEP);
			this->counter_504[apOther->owner->which_player] = 0xA;
		} else {
			this->dEn_c::playerCollision(apThis, apOther);
			this->_vf220(apOther->owner);
			if (Player_VF3D4(player)) {
				// WE'VE GOT A STAR, FOLKS
				if (apThis->_18 == 1 && !player->collMgr.isOnTopOfTile() && player->pos.y > apThis->bottom()) {
					bouncePlayer(player, 3.0f);
					PlaySound(this, SE_EMY_CMN_STEP);
				} else {
					dAcPy_vf3F8(player, this, 3);
				}
				this->counter_504[apOther->owner->which_player] = 0xA;

			}
		} 
	}			
	void daShyGuyGiant::_vf278(void *other) {
		PlaySound(this, SE_EMY_HANACHAN_STOMP);
}

	void daShyGuyGiant::bouncePlayerWhenJumpedOn(void *player) {
		bouncePlayer(player, 5.0f);
	}

	void daShyGuyGiant::yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
		this->playerCollision(apThis, apOther);
	}
	bool daShyGuyGiant::collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther) {
		return BigHanaWeirdGP(this, apThis, apOther);
	}
	bool daShyGuyGiant::collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther) {
		return BigHanaGroundPound(this, apThis, apOther);
	}
	bool daShyGuyGiant::collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther) {
		return BigHanaGroundPound(this, apThis, apOther);
	}

	bool daShyGuyGiant::collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther) {
		apOther->owner->kill();
		return true;
	}
	bool daShyGuyGiant::collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther){
		dAcPy_vf3F8(apOther->owner, this, 3);
		this->counter_504[apOther->owner->which_player] = 0xA;
		return true;
	}
	bool daShyGuyGiant::collisionCat5_Mario(ActivePhysics *apThis, ActivePhysics *apOther){
		return true;
	}
	bool daShyGuyGiant::collisionCat11_PipeCannon(ActivePhysics *apThis, ActivePhysics *apOther){
		return true;
	}
	bool daShyGuyGiant::collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther) {
		return true;
	}
	bool daShyGuyGiant::collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther){
		/*int hitType = usedForDeterminingStatePress_or_playerCollision(this, apThis, apOther, 0);
		if (hitType == 1 || hitType == 3) {
			PlaySound(this, SE_EMY_CMN_STEP);
			bouncePlayerWhenJumpedOn(apOther->owner);
		} else {
			dAcPy_vf3F8(apOther->owner, this, 3);
		}

		this->counter_504[apOther->owner->which_player] = 0xA;
		return true;*/
		return false;
	}

	bool daShyGuyGiant::collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther){
		return BigHanaFireball(this, apThis, apOther);
	}
	bool daShyGuyGiant::collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther) {
		return BigHanaFireball(this, apThis, apOther);
	}
	bool daShyGuyGiant::collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther) {
		return BigHanaIceball(this, apThis, apOther);
	}

	// These handle the ice crap
	void daShyGuyGiant::_vf148() {
		dEn_c::_vf148();
		doStateChange(&StateID_Die);
	}
	void daShyGuyGiant::_vf14C() {
		dEn_c::_vf14C();
		doStateChange(&StateID_Die);
	}

	extern "C" void sub_80024C20(void);
	extern "C" void __destroy_arr(void*, void(*)(void), int, int);

	bool daShyGuyGiant::CreateIceActors() {
		struct DoSomethingCool my_struct = { 0, this->pos, {2.4, 3.0, 3.0}, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
		if (type == 1) { my_struct.scale = (Vec3){4.8, 6.0, 6.0}; }
		if (type == 2) { my_struct.scale = (Vec3){7.2, 9.0, 9.0}; }
	    this->frzMgr.Create_ICEACTORs( (void*)&my_struct, 1 );
	    __destroy_arr( (void*)&my_struct, sub_80024C20, 0x3C, 1 );
	    return true;
	}

	void daShyGuyGiant::addScoreWhenHit(void *other) { }

	void daShyGuyGiant::spawnHitEffectAtPosition(Vec2 pos) { }
	void daShyGuyGiant::doSomethingWithHardHitAndSoftHitEffects(Vec pos) { }
	void daShyGuyGiant::playEnemyDownSound2() { }
	void daShyGuyGiant::playHpdpSound1() { } // plays PLAYER_SE_EMY/GROUP_BOOT/SE_EMY_DOWN_HPDP_S or _H
	void daShyGuyGiant::playEnemyDownSound1() { }
	void daShyGuyGiant::playEnemyDownComboSound(void *player) { } // AcPy_c/daPlBase_c?
	void daShyGuyGiant::playHpdpSound2() { } // plays PLAYER_SE_EMY/GROUP_BOOT/SE_EMY_DOWN_HPDP_S or _H
	void daShyGuyGiant::_vf260(void *other) { } // AcPy/PlBase? plays the SE_EMY_FUMU_%d sounds based on some value
	void daShyGuyGiant::_vf264(dStageActor_c *other) { } // if other is player or yoshi, do Wm_en_hit and a few other things
	void daShyGuyGiant::_vf268(void *other) { } // AcPy/PlBase? plays the SE_EMY_DOWN_SPIN_%d sounds based on some value

	void daShyGuyGiant::powBlockActivated(bool isNotMPGP) {
	}

bool daShyGuyGiant::calculateTileCollisions() {
	// Returns true if sprite should turn, false if not.

	HandleXSpeed();
	HandleYSpeed();
	doSpriteMovement();

	cmgr_returnValue = collMgr.isOnTopOfTile();
	collMgr.calculateBelowCollisionWithSmokeEffect();

	if (isBouncing) {
		stuffRelatingToCollisions(0.1875f, 1.0f, 0.5f);
		if (speed.y != 0.0f)
			isBouncing = false;
	}

	float xDelta = pos.x - last_pos.x;
	if (xDelta >= 0.0f)
		direction = 0;
	else
		direction = 1;

	if (collMgr.isOnTopOfTile()) {
		// Walking into a tile branch

		if (cmgr_returnValue == 0)
			isBouncing = true;

		if (speed.x != 0.0f) {
			//playWmEnIronEffect();
		}

		speed.y = 0.0f;

		// u32 blah = collMgr.s_80070760();
		// u8 one = (blah & 0xFF);
		// static const float incs[5] = {0.00390625f, 0.0078125f, 0.015625f, 0.0234375f, 0.03125f};
		// x_speed_inc = incs[one];
		max_speed.x = (direction == 1) ? -this->XSpeed : this->XSpeed;
	} else {
		x_speed_inc = 0.0f;
	}

	// Bouncing checks
	if (_34A & 4) {
		Vec v = (Vec){0.0f, 1.0f, 0.0f};
		collMgr.pSpeed = &v;

		if (collMgr.calculateAboveCollision(collMgr.outputMaybe))
			speed.y = 0.0f;

		collMgr.pSpeed = &speed;

	} else {
		if (collMgr.calculateAboveCollision(collMgr.outputMaybe))
			speed.y = 0.0f;
	}

	collMgr.calculateAdjacentCollision(0);

	// Switch Direction
	if (collMgr.outputMaybe & (0x15 << direction)) {
		if (collMgr.isOnTopOfTile()) {
			isBouncing = true;
		}
		return true;
	}
	return false;
}

void daShyGuyGiant::bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate) {
	nw4r::g3d::ResAnmChr anmChr = this->anmFile.GetResAnmChr(name);
	this->chrAnimation.bind(&this->bodyModel, anmChr, unk);
	this->bodyModel.bindAnim(&this->chrAnimation, unk2);
	this->chrAnimation.setUpdateRate(rate);
}

int daShyGuyGiant::onCreate() {
	OSReport("Spawning Shy Guy");
	this->type = (this->settings >> 28) & 0xF;

	allocator.link(-1, GameHeaps[0], 0, 0x20);

	u32 colour = (this->settings >> 24) & 0xF;

	// Shy Guy Colours
	if (colour == 1) {
		this->resFile.data = getResource("shyguy", "g3d/ShyGuyBlue.brres");
	}
	else if (colour == 2) {
		this->resFile.data = getResource("shyguy", "g3d/ShyGuyGreen.brres");
	}
	else if (colour == 3) {
		this->resFile.data = getResource("shyguy", "g3d/ShyGuyCyan.brres");
	}
	else if (colour == 4) {
		this->resFile.data = getResource("shyguy", "g3d/ShyGuyPurple.brres");
	}
	else {
		this->resFile.data = getResource("shyguy", "g3d/ShyGuyRed.brres");
	}
	nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("body_h");
	bodyModel.setup(mdl, &allocator, 0x224, 1, 0);


	// Animations start here
	this->anmFile.data = getResource("shyguy", "g3d/ShyGuyAnimations.brres");
	nw4r::g3d::ResAnmChr anmChr = this->anmFile.GetResAnmChr("c18_IDLE_R");
	this->chrAnimation.setup(mdl, anmChr, &this->allocator, 0);

	allocator.unlink();

	// Stuff I do understand

	this->pos.y += 32.0;
	this->rot.x = 0; // X is vertical axis
	this->rot.y = 0xD800; // Y is horizontal axis
	this->rot.z = 0; // Z is ... an axis >.>
	this->direction = 1; // Heading left.
	
	this->speed.x = 0.0;
	this->speed.y = 0.0;
	this->Baseline = this->pos.y;

	ActivePhysics::Info HitMeBaby;
	float anmSpeed;

	if (type == 0) {
		this->scale = (Vec){40.0f, 40.0f, 40.0f};

		HitMeBaby.xDistToCenter = 0.0;
		HitMeBaby.yDistToCenter = 20.0;

		HitMeBaby.xDistToEdge = 14.0;
		HitMeBaby.yDistToEdge = 20.0;

		this->XSpeed = 0.4;
		anmSpeed = 1.0;

		static const lineSensor_s below(12<<12, 4<<12, 0<<12);
		static const lineSensor_s adjacent(14<<12, 9<<12, 14<<12);
		collMgr.init(this, &below, 0, &adjacent);
	}
	else if (type == 1) {
		this->scale = (Vec){80.0f, 80.0f, 80.0f};

		HitMeBaby.xDistToCenter = 0.0;
		HitMeBaby.yDistToCenter = 40.0;

		HitMeBaby.xDistToEdge = 28.0;
		HitMeBaby.yDistToEdge = 40.0;

		this->XSpeed = 0.4;
		anmSpeed = 0.5;

		static const lineSensor_s below(12<<12, 4<<12, 0<<12);
		static const lineSensor_s adjacent(28<<12, 9<<12, 28<<12);
		collMgr.init(this, &below, 0, &adjacent);
	}
	else {
		this->scale = (Vec){120.0f, 120.0f, 120.0f};

		HitMeBaby.xDistToCenter = 0.0;
		HitMeBaby.yDistToCenter = 60.0;

		HitMeBaby.xDistToEdge = 42.0;
		HitMeBaby.yDistToEdge = 60.0;

		this->XSpeed = 0.4;
		anmSpeed = 0.25;	

		static const lineSensor_s below(12<<12, 4<<12, 0<<12);
		static const lineSensor_s adjacent(42<<12, 9<<12, 42<<12);
		collMgr.init(this, &below, 0, &adjacent);
	}

	HitMeBaby.category1 = 0x3;
	HitMeBaby.category2 = 0x9;
	HitMeBaby.bitfield1 = 0x4F;
	HitMeBaby.bitfield2 = 0xffba7ffe;
	HitMeBaby.unkShort1C = 0x20000;
	HitMeBaby.callback = &shyCollisionCallback;

	this->aPhysics.initWithStruct(this, &HitMeBaby);
	this->aPhysics.addToList();

	collMgr.calculateBelowCollisionWithSmokeEffect();

	cmgr_returnValue = collMgr.isOnTopOfTile();

	if (collMgr.isOnTopOfTile())
		isBouncing = false;
	else
		isBouncing = true;

	// State Changer
	bindAnimChr_and_setUpdateRate("c18_EV_WIN_2_R", 1, 0.0, anmSpeed); 
	doStateChange(&StateID_RealWalk);

	this->onExecute();
	return true;
}

int daShyGuyGiant::onDelete() {
	return true;
}

int daShyGuyGiant::onExecute() {
	acState.execute();
	updateModelMatrices();

	return true;
}

int daShyGuyGiant::onDraw() {
	bodyModel.scheduleForDrawing();
	return true;
}

void daShyGuyGiant::updateModelMatrices() {
	float yoff;

	if (type == 0) 		{ yoff = -5.0; }
	else if (type == 1) { yoff = -10.0; }
	else 				{ yoff = -15.0; }

	matrix.translation(pos.x, pos.y + yoff, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	bodyModel.setDrawMatrix(matrix);
	bodyModel.setScale(&scale);
	bodyModel.calcWorld(false);
}


///////////////
// Real Walk State
///////////////
	void daShyGuyGiant::beginState_RealWalk() {
		//inline this piece of code
		this->max_speed.x = (this->direction) ? -this->XSpeed : this->XSpeed;
		this->speed.x = (direction) ? -this->XSpeed : this->XSpeed;

		this->max_speed.y = -4.0;
		this->speed.y = -4.0;
		this->y_speed_inc = -0.1875;
	}
	void daShyGuyGiant::executeState_RealWalk() { 
		bodyModel._vf1C();

		bool ret = calculateTileCollisions();
		if (ret) {
			doStateChange(&StateID_RealTurn);
		}

		if(this->chrAnimation.isAnimationDone()) {
			this->chrAnimation.setCurrentFrame(0.0);
		}
	}
	void daShyGuyGiant::endState_RealWalk() { }

///////////////
// Real Turn State
///////////////
	void daShyGuyGiant::beginState_RealTurn() {

		this->direction ^= 1;
		this->speed.x = 0.0;
	}
	void daShyGuyGiant::executeState_RealTurn() { 
		bodyModel._vf1C();

		if(this->chrAnimation.isAnimationDone()) {
			this->chrAnimation.setCurrentFrame(0.0);
		}

		u16 amt = (this->direction == 0) ? 0x2800 : 0xD800;
		int done = SmoothRotation(&this->rot.y, amt, 0x800);

		if(done) {
			this->doStateChange(&StateID_RealWalk);
		}
	}
	void daShyGuyGiant::endState_RealTurn() {
	}

///////////////
// Die State
///////////////
	void daShyGuyGiant::beginState_Die() {
		// dEn_c::dieFall_Begin();
		this->removeMyActivePhysics();

		bindAnimChr_and_setUpdateRate("c18_C_BLOCK_BREAK_R", 1, 0.0, 2.0); 
		this->timer = 0;
		this->dying = -10.0;
		this->Baseline = this->pos.y;
		this->rot.y = 0;
		this->rot.x = 0;
	}
	void daShyGuyGiant::executeState_Die() { 
		bodyModel._vf1C();

		if(this->chrAnimation.isAnimationDone()) {
			this->chrAnimation.setCurrentFrame(0.0);
		}

		this->timer += 1;
		 		
		// this->pos.x += 0.5; 
		this->pos.y = Baseline + (-0.2 * dying * dying) + 20.0;
		
		this->dying += 0.5;
			
		if (this->timer > 450) {
			this->kill();
			this->Delete(this->deleteForever);
		}

		// dEn_c::dieFall_Execute();

	}
	void daShyGuyGiant::endState_Die() {
	}


//
// processed\../src/meteor.cpp
//

#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include "boss.h"

class dMeteor : public dEn_c {
	int onCreate();
	int onDelete();
	int onExecute();
	int onDraw();

	static dMeteor *build();

	mHeapAllocator_c allocator;
	m3d::mdl_c bodyModel;
	nw4r::g3d::ResFile resFile;
	mEf::es2 effect;

	int timer;
	int spinSpeed;
	char spinDir;
	char isElectric;

	Physics MakeItRound;

	void updateModelMatrices();
	void playerCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther);

	public:
		void kill();
};

dMeteor *dMeteor::build() {
	void *buffer = AllocFromGameHeap1(sizeof(dMeteor));
	return new(buffer) dMeteor;
}

const char* MEarcNameList [] = {
	"kazan_rock",
	NULL	
};

// extern "C" dStageActor_c *GetSpecificPlayerActor(int num);
// extern "C" void *modifyPlayerPropertiesWithRollingObject(dStageActor_c *Player, float _52C);


void dMeteor::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther) { 
	DamagePlayer(this, apThis, apOther);
}

void MeteorPhysicsCallback(dMeteor *self, dEn_c *other) {
	if (other->name == 657) {
		OSReport("CANNON COLLISION");

		SpawnEffect("Wm_en_explosion", 0, &other->pos, &(S16Vec){0,0,0}, &(Vec){1.0, 1.0, 1.0});
		SpawnEffect("Wm_en_explosion_smk", 0, &other->pos, &(S16Vec){0,0,0}, &(Vec){1.0, 1.0, 1.0});
		PlaySound(other, SE_OBJ_TARU_BREAK);
		other->Delete(1);

		switch ((self->settings >> 24) & 3) {
			case 1:
				dStageActor_c::create(EN_HATENA_BALLOON, 0x100, &self->pos, 0, self->currentLayerID);
				break;
			case 2:
				VEC3 coinPos = {self->pos.x - 16.0f, self->pos.y, self->pos.z};
				dStageActor_c::create(EN_COIN, 9, &coinPos, 0, self->currentLayerID);
				break;
		}

		self->kill();
	}
}

bool dMeteor::collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther) { 
	DamagePlayer(this, apThis, apOther);
	return true;
}


int dMeteor::onCreate() {

	// Setup Model
	allocator.link(-1, GameHeaps[0], 0, 0x20);

	this->resFile.data = getResource("kazan_rock", "g3d/kazan_rock.brres");
	nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("kazan_rock");
	bodyModel.setup(mdl, &allocator, 0x224, 1, 0);
	SetupTextures_Enemy(&bodyModel, 0);

	allocator.unlink();


	// Retrieve Scale and set it up
	float sca = (float)((this->settings >> 8) & 0xFF);
	sca = (sca/5.0) + 0.2;

	this->scale = (Vec){sca,sca,sca};

	// Other settings
	this->spinDir = this->settings & 0x1;
	this->spinSpeed = ((this->settings >> 16) & 0xFF) * 20;
	this->isElectric = (this->settings >> 4) & 0x1;


	// Setup Physics
	if (isElectric) {
		ActivePhysics::Info elec;
		elec.xDistToCenter = 0.0;
		elec.yDistToCenter = 0.0;

		elec.xDistToEdge = 13.0 * sca;
		elec.yDistToEdge = 13.0 * sca;

		elec.category1 = 0x3;
		elec.category2 = 0x0;
		elec.bitfield1 = 0x4F;
		elec.bitfield2 = 0x200;
		elec.unkShort1C = 0;
		elec.callback = &dEn_c::collisionCallback;

		this->aPhysics.initWithStruct(this, &elec);
		this->aPhysics.addToList();	
	}

	MakeItRound.baseSetup(this, &MeteorPhysicsCallback, &MeteorPhysicsCallback, &MeteorPhysicsCallback, 1, 0);

	MakeItRound.x = 0.0;
	MakeItRound.y = 0.0;

	MakeItRound.diameter = 13.0 * sca;
	MakeItRound.isRound = 1;

	MakeItRound.update();

	MakeItRound.addToList();

	this->pos.z = (settings & 0x1000000) ? -2000.0f : 3458.0f;
		
	this->onExecute();
	return true;
}

int dMeteor::onDelete() {
	return true;
}

int dMeteor::onExecute() {

	if (spinDir == 0) 	{ rot.z -= spinSpeed; }
	else 				{ rot.z += spinSpeed; }

	MakeItRound.update();
	updateModelMatrices();

	if (isElectric) {
		effect.spawn("Wm_en_birikyu_biri", 0, &(Vec){pos.x, pos.y, pos.z+500.0}, &rot, &(Vec){scale.x*0.8, scale.y*0.8, scale.z*0.8});
		PlaySound(this, SE_EMY_BIRIKYU_SPARK);
	}

	// for (i=0; i<4; i++) {
	// 	dStageActor_c *player = GetSpecificPlayerActor(i);
	// 	modifyPlayerPropertiesWithRollingObject(player, );
	// }

	return true;
}

int dMeteor::onDraw() {

	bodyModel.scheduleForDrawing();
	bodyModel._vf1C();
	return true;
}

void dMeteor::updateModelMatrices() {
	// This won't work with wrap because I'm lazy.
	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	bodyModel.setDrawMatrix(matrix);
	bodyModel.setScale(&scale);
	bodyModel.calcWorld(false);
}

void dMeteor::kill() {
	PlaySound(this, SE_OBJ_ROCK_LAND);
	SpawnEffect("Wm_ob_cmnboxsmoke", 0, &pos, &rot, &scale);
	SpawnEffect("Wm_ob_cmnboxgrain", 0, &pos, &rot, &scale);

	this->Delete(1);
}



//
// processed\../src/electricLine.cpp
//

#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>

class daElectricLine : public dEn_c {
	int onCreate();
	int onDelete();
	int onExecute();
	int onDraw();

	mHeapAllocator_c allocator;
	nw4r::g3d::ResFile resFile;

	dEn_c *Needles;
	u32 delay;
	u32 timer;
	char loops;

	static daElectricLine *build();

	USING_STATES(daElectricLine);
	DECLARE_STATE(Activate);
	DECLARE_STATE(Deactivate);
	DECLARE_STATE(Die);
};

daElectricLine *daElectricLine::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daElectricLine));
	return new(buffer) daElectricLine;
}

///////////////////////
// Externs and States
///////////////////////


	CREATE_STATE(daElectricLine, Activate);
	CREATE_STATE(daElectricLine, Deactivate);
	CREATE_STATE(daElectricLine, Die);



int daElectricLine::onCreate() {

	Vec temppos = this->pos;
	temppos.x += 24.0;

	// Settings for rotation: 0 = facing right, 1 = facing left, 2 = facing up, 3 = facing down
	char settings = 0;
	if (this->settings & 0x1) {
		settings = 1;
		temppos.x -= 32.0;
	}


	Needles = (daNeedles*)create(NEEDLE_FOR_KOOPA_JR_B, settings, &temppos, &this->rot, 0);
	Needles->doStateChange(&daNeedles::StateID_DemoWait);
	
	// Needles->aPhysics.info.category1 = 0x3;
	// Needles->aPhysics.info.bitfield1 = 0x4F;
	// Needles->aPhysics.info.bitfield2 = 0xffbafffe;

	// Delay in 1/6ths of a second
	this->delay = (this->settings >> 16) * 10;
	this->loops = (this->settings >> 4);

	// State Changers
	doStateChange(&StateID_Activate);

	this->onExecute();
	return true;
}

int daElectricLine::onDelete() {
	return true;
}

int daElectricLine::onExecute() {
	acState.execute();	
	return true;
}

int daElectricLine::onDraw() {
	return true;
}


// States:
//
// DemoWait - all nullsubs, does nothing
// DemoAwake - moves the spikes in their respective directions
// Idle - Fires off an infinity of effects for some reason.
// Die - removes physics, then nullsubs


///////////////
// Activate State
///////////////
	void daElectricLine::beginState_Activate() { 
		this->timer = this->delay;
		Needles->doStateChange(&daNeedles::StateID_Idle);
	}
	void daElectricLine::executeState_Activate() { 
		if (this->loops) {
			this->timer--;
			if (this->timer == 0) {
				this->loops += 1;
				doStateChange(&StateID_Deactivate);
			}
		}
	}
	void daElectricLine::endState_Activate() { }

///////////////
// Deactivate State
///////////////
	void daElectricLine::beginState_Deactivate() { 
		this->timer = this->delay; 
		Needles->removeMyActivePhysics();
		Needles->doStateChange(&daNeedles::StateID_DemoWait);
	}
	void daElectricLine::executeState_Deactivate() { 

		this->timer--;
		if (this->timer == 0) {
			doStateChange(&StateID_Activate);
		}
	}
	void daElectricLine::endState_Deactivate() { 
		Needles->addMyActivePhysics();
	}


///////////////
// Die State
///////////////
	void daElectricLine::beginState_Die() { Needles->doStateChange(&daNeedles::StateID_Die); }
	void daElectricLine::executeState_Die() { }
	void daElectricLine::endState_Die() { }


//
// processed\../src/thundercloud.cpp
//

#include <common.h>
#include <game.h>
#include <g3dhax.h>

#include "boss.h"

const char* TLCarcNameList [] = {
	"tcloud",
	NULL	
};

class dThunderCloud : public dEn_c {
	int onCreate();
	int onDelete();
	int onExecute();
	int onDraw();

	mHeapAllocator_c allocator;
	m3d::mdl_c bodyModel;
	nw4r::g3d::ResFile resFile;
	m3d::anmChr_c anm;

	mEf::es2 bolt;
	mEf::es2 charge;

	float Baseline;
	u32 timer;
	int dying;
	char killFlag;
	bool stationary;
	float leader;
	pointSensor_s below;

	bool usingEvents;
	u64 eventFlag;

	ActivePhysics Lightning;

	void dieFall_Begin();
	void dieFall_Execute();
	static dThunderCloud *build();

	void updateModelMatrices();

	void playerCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	void yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat5_Mario(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat11_PipeCannon(ActivePhysics *apThis, ActivePhysics *apOther);

	void bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate);

	void powBlockActivated(bool isNotMPGP);

	void _vf148();
	void _vf14C();
	bool CreateIceActors();

	void lightningStrike();

	USING_STATES(dThunderCloud);
	DECLARE_STATE(Follow);
	DECLARE_STATE(Lightning);
	DECLARE_STATE(Wait);
};

dThunderCloud *dThunderCloud::build() {
	void *buffer = AllocFromGameHeap1(sizeof(dThunderCloud));
	return new(buffer) dThunderCloud;
}


CREATE_STATE(dThunderCloud, Follow);
CREATE_STATE(dThunderCloud, Lightning);
CREATE_STATE(dThunderCloud, Wait);

void dThunderCloud::powBlockActivated(bool isNotMPGP) { }


// Collision Callbacks
	extern "C" void dAcPy_vf3F4(void* mario, void* other, int t);
	extern "C" bool BigHanaFireball(dEn_c* t, ActivePhysics *apThis, ActivePhysics *apOther);
	extern "C" void *dAcPy_c__ChangePowerupWithAnimation(void * Player, int powerup);
	extern "C" int CheckExistingPowerup(void * Player);

	void dThunderCloud::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther) { 

		if (this->counter_504[apOther->owner->which_player]) {
			if (apThis->info.category2 == 0x9) {
				int p = CheckExistingPowerup(apOther->owner);
				if (p != 3) {	// Powerups - 0 = small; 1 = big; 2 = fire; 3 = mini; 4 = prop; 5 = peng; 6 = ice; 7 = hammer
					dAcPy_c__ChangePowerupWithAnimation(apOther->owner, 3);
				}
				else { dAcPy_vf3F4(apOther->owner, this, 9); }
			}

			else { dAcPy_vf3F4(apOther->owner, this, 9); }
		}

		this->counter_504[apOther->owner->which_player] = 0x20;
	}

	void dThunderCloud::yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther) { this->playerCollision(apThis, apOther); }
	bool dThunderCloud::collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther) { this->playerCollision(apThis, apOther); return true; }
	bool dThunderCloud::collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther) { this->playerCollision(apThis, apOther); return true; }
	bool dThunderCloud::collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther) { this->playerCollision(apThis, apOther); return true; }
	bool dThunderCloud::collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther) { this->playerCollision(apThis, apOther); return true; }
	bool dThunderCloud::collisionCat5_Mario(ActivePhysics *apThis, ActivePhysics *apOther) { this->playerCollision(apThis, apOther); return true; }
	bool dThunderCloud::collisionCat11_PipeCannon(ActivePhysics *apThis, ActivePhysics *apOther) { this->playerCollision(apThis, apOther); return true; }

	bool dThunderCloud::collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther) { return BigHanaFireball(this, apThis, apOther); }
	bool dThunderCloud::collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther) { return BigHanaFireball(this, apThis, apOther); }

	bool dThunderCloud::collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther) { 
		if (apThis->info.category2 == 0x9) { return true; }
		PlaySound(this, SE_EMY_DOWN);
		doStateChange(&StateID_DieFall);
		return true;
	}
	bool dThunderCloud::collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther) {
		this->collisionCat13_Hammer(apThis, apOther);
		return true;
	}
	bool dThunderCloud::collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther) { 
		if (apThis->info.category2 == 0x9) { return true; }
		dEn_c::collisionCat3_StarPower(apThis, apOther);
		this->collisionCat13_Hammer(apThis, apOther);
		return true;
	}


	// These handle the ice crap
	void dThunderCloud::_vf148() {
		dEn_c::_vf148();
		doStateChange(&StateID_DieFall);
	}
	void dThunderCloud::_vf14C() {
		dEn_c::_vf14C();
		doStateChange(&StateID_DieFall);
	}

	extern "C" void sub_80024C20(void);
	extern "C" void __destroy_arr(void*, void(*)(void), int, int);
	//extern "C" __destroy_arr(struct DoSomethingCool, void(*)(void), int cnt, int bar);

	bool dThunderCloud::CreateIceActors() {
		this->Lightning.removeFromList();

		struct DoSomethingCool my_struct = { 0, (Vec){pos.x, pos.y - 16.0, pos.z}, {1.75, 1.4, 1.5}, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
	    this->frzMgr.Create_ICEACTORs( (void*)&my_struct, 1 );
	    __destroy_arr( (void*)&my_struct, sub_80024C20, 0x3C, 1 );
	    return true;
	}


void dThunderCloud::dieFall_Begin() {
	this->Lightning.removeFromList();
	this->timer = 0; 
	this->dEn_c::dieFall_Begin();
}
void dThunderCloud::dieFall_Execute() {	
	if (this->killFlag == 1) { return; }

	this->timer = this->timer + 1;
	 
	this->dying = this->dying + 0.15;
	
	this->pos.x = this->pos.x + 0.15;
	this->pos.y = this->pos.y - ((-0.2 * (this->dying*this->dying)) + 5);
	
	this->dEn_c::dieFall_Execute();
		
	if (this->timer > 450) {
		
		if (((this->settings >> 28) > 0) || (stationary)) { 		
			this->Delete(1);
			this->killFlag = 1;
			return;
		}
		
		dStageActor_c *Player = GetSpecificPlayerActor(0);
		if (Player == 0) { Player = GetSpecificPlayerActor(1); }
		if (Player == 0) { Player = GetSpecificPlayerActor(2); }
		if (Player == 0) { Player = GetSpecificPlayerActor(3); }
		

		if (Player == 0) { 
			this->pos.x = 0;
		} else {
			this->pos.x = Player->pos.x - 300;
		}
				
		this->pos.y = this->Baseline; 

		SpawnEffect("Wm_en_blockcloud", 0, &pos, &(const S16Vec){0,0,0}, &(const Vec){1.0f,1.0f,1.0f});
		
		scale.x = scale.y = scale.z = 0.0f;
		this->aPhysics.addToList();
		doStateChange(&StateID_Follow);
	}
}


void dThunderCloud::bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate) {
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr(name);
	this->anm.bind(&this->bodyModel, anmChr, unk);
	this->bodyModel.bindAnim(&this->anm, unk2);
	this->anm.setUpdateRate(rate);
}

int dThunderCloud::onCreate() {

	// Setup the model
	allocator.link(-1, GameHeaps[0], 0, 0x20);

	this->resFile.data = getResource("tcloud", "g3d/tcloud.brres");
	nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("cloud");
	bodyModel.setup(mdl, &allocator, 0x224, 1, 0);
	SetupTextures_Enemy(&bodyModel, 0);

	bool ret;
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr("cloud_wait");
	ret = this->anm.setup(mdl, anmChr, &this->allocator, 0);

	allocator.unlink();



	scale = (Vec){1.2f, 1.2f, 1.2f};

	// Scale and Physics
	ActivePhysics::Info Cloud;
	Cloud.xDistToCenter = 0.0;
	Cloud.yDistToCenter = 0.0;
	Cloud.category1 = 0x3;
	Cloud.category2 = 0x0;
	Cloud.bitfield1 = 0x4F;

	Cloud.bitfield2 = 0xffba7ffe; 
	Cloud.xDistToEdge = 18.0f * scale.x;
	Cloud.yDistToEdge = 12.0f * scale.y;

	Cloud.unkShort1C = 0;
	Cloud.callback = &dEn_c::collisionCallback;

	this->aPhysics.initWithStruct(this, &Cloud);
	this->aPhysics.addToList();

	below.x = 0;
	below.y = 0;
	collMgr.init(this, &below, 0, 0);

	// Some Settings
	this->Baseline = this->pos.y;
	this->dying = -5;
	this->killFlag = 0;
	this->pos.z = 5750.0f; // sun

	stationary 		= this->settings & 0xF;
	
	char eventNum	= (this->settings >> 16) & 0xFF;
	usingEvents = (stationary != 0) && (eventNum != 0);

	this->eventFlag = (u64)1 << (eventNum - 1);


	// State Change!
	if (stationary) { doStateChange(&StateID_Wait); }
	else 			{ doStateChange(&StateID_Follow); }

	this->onExecute();
	return true;
}

int dThunderCloud::onDelete() {
	return true;
}

int dThunderCloud::onExecute() {
	if (scale.x < 1.0f)
		scale.x = scale.y = scale.z = scale.x + 0.0375f;
	else
		scale.x = scale.y = scale.z = 1.0f;

	acState.execute();
	updateModelMatrices();
	bodyModel._vf1C();

	if ((dFlagMgr_c::instance->flags & this->eventFlag) && (!stationary)) {
		if (this->killFlag == 0 && acState.getCurrentState()->isNotEqual(&StateID_DieFall)) {
			this->kill();
			this->pos.y = this->pos.y + 800.0; 
			this->killFlag = 1;
			doStateChange(&StateID_DieFall);
		}
	}
		
	return true;
}

int dThunderCloud::onDraw() {
	bodyModel.scheduleForDrawing();

	return true;
}


void dThunderCloud::updateModelMatrices() {
	// This won't work with wrap because I'm lazy.
	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	bodyModel.setDrawMatrix(matrix);
	bodyModel.setScale(&scale);
	bodyModel.calcWorld(false);
}


// Follow State

void dThunderCloud::beginState_Follow() { 
	this->timer = 0;
	this->bindAnimChr_and_setUpdateRate("cloud_wait", 1, 0.0, 1.0);
	this->rot.x = 0;
	this->rot.y = 0;
	this->rot.z = 0;
	PlaySound(this, SE_AMB_THUNDER_CLOUD);
}
void dThunderCloud::executeState_Follow() {

	charge.spawn("Wm_mr_electricshock_biri02_s", 0, &pos, &(S16Vec){0,0,0}, &(Vec){1.5, 1.5, 1.5});

	if(this->anm.isAnimationDone()) {
		this->anm.setCurrentFrame(0.0); }

	if (this->timer > 200) { this->doStateChange(&StateID_Lightning); }

	this->direction = dSprite_c__getXDirectionOfFurthestPlayerRelativeToVEC3(this, this->pos);
	
	float speedDelta;
	speedDelta = 0.05;

	if (this->direction == 0) { // Going Left
		this->speed.x = this->speed.x + speedDelta; // 
		
		if (this->speed.x < 0) { this->speed.x += (speedDelta / 1.5); }
		if (this->speed.x < -6.0) { this->speed.x += (speedDelta * 2.0); }
	}
	else { // Going Right
		this->speed.x = this->speed.x - speedDelta;

		if (this->speed.x > 0) { this->speed.x -= (speedDelta / 1.5); }
		if (this->speed.x > 6.0) { this->speed.x -= (speedDelta * 2.0); }
	}
	
	this->HandleXSpeed();
	
	float yDiff;
	yDiff = (this->Baseline - this->pos.y) / 8;
	this->speed.y = yDiff;
		
	this->HandleYSpeed();

	this->UpdateObjectPosBasedOnSpeedValuesReal();

	this->timer = this->timer + 1;
}
void dThunderCloud::endState_Follow() { 
	this->speed.y = 0;
}


// Wait State

void dThunderCloud::beginState_Wait() { 
	this->timer = 0;
	this->bindAnimChr_and_setUpdateRate("cloud_wait", 1, 0.0, 1.0);
	this->rot.x = 0;
	this->rot.y = 0;
	this->rot.z = 0;
	PlaySound(this, SE_AMB_THUNDER_CLOUD);
}
void dThunderCloud::executeState_Wait() {

	charge.spawn("Wm_mr_electricshock_biri02_s", 0, &pos, &(S16Vec){0,0,0}, &(Vec){1.5, 1.5, 1.5});

	if(this->anm.isAnimationDone()) {
		this->anm.setCurrentFrame(0.0); }

	if ((this->settings >> 16) & 0xFF) {
		if (dFlagMgr_c::instance->flags & this->eventFlag) {
			this->doStateChange(&StateID_Lightning);
		}
	}
	else {
		if (this->timer > 200) { this->doStateChange(&StateID_Lightning); }
		timer += 1;
	}
}
void dThunderCloud::endState_Wait() { }


// Lightning State
static void lightningCallback(ActivePhysics *one, ActivePhysics *two) {
	if (one->owner->name == WM_BUBBLE && two->owner->name == WM_BUBBLE)
		return;
	dEn_c::collisionCallback(one, two);
}

void dThunderCloud::lightningStrike() {
	PlaySound(this, SE_OBJ_KAZAN_ERUPTION); 

	float boltsize = (leader-14.0)/2;
	float boltpos = -boltsize - 14.0;

	ActivePhysics::Info Shock;
	Shock.xDistToCenter = 0.0;
	Shock.yDistToCenter = boltpos;
	Shock.category1 = 0x3;
	Shock.category2 = 0x9;
	Shock.bitfield1 = 0x4D;

	Shock.bitfield2 = 0x420; 
	Shock.xDistToEdge = 12.0;
	Shock.yDistToEdge = boltsize;

	Shock.unkShort1C = 0;
	Shock.callback = &dEn_c::collisionCallback;

	this->Lightning.initWithStruct(this, &Shock);
	this->Lightning.addToList();
}

void dThunderCloud::beginState_Lightning() {
	float backupY = pos.y, backupYSpeed = speed.y;

	speed.x = 0.0;
	speed.y = -1.0f;

	u32 result = 0;
	while (result == 0 && below.y > (-30 << 16)) {
		pos.y = backupY;
		below.y -= 0x4000;
		//OSReport("Sending out leader to %d", below.y>>12);

		result = collMgr.calculateBelowCollisionWithSmokeEffect();
		if (result == 0) {
			u32 tb1 = collMgr.getTileBehaviour1At(pos.x, pos.y + (below.y >> 12), 0);
			if (tb1 & 0x8000 && !(tb1 & 0x20))
				result = 1;
		}
		//OSReport("Result %d", result);
	}

	if (result == 0) {
		OSReport("Couldn't find any ground, falling back to 13 tiles distance");

		leader = 13 * 16;
	} else {
		OSReport("Lightning strikes at %d", below.y>>12);

		leader = -(below.y >> 12);
	}
	below.y = 0;

	pos.y = backupY;
	speed.y = backupYSpeed;

	if (usingEvents) {
		timer = 2;
		this->bindAnimChr_and_setUpdateRate("thundershoot", 1, 0.0, 1.0); 
		lightningStrike();
	} else {
		timer = 0;
	}
}
void dThunderCloud::executeState_Lightning() { 

	switch (timer) {
		case 0:
			charge.spawn("Wm_en_birikyu", 0, &(Vec){pos.x, pos.y, pos.z}, &(S16Vec){0,0,0}, &(Vec){1.5, 1.5, 1.5});
			break;
		case 1:
			charge.spawn("Wm_en_birikyu", 0, &(Vec){pos.x, pos.y, pos.z}, &(S16Vec){0,0,0}, &(Vec){1.5, 1.5, 1.5});
			break;
		case 2:		
			PlaySound(this, SE_BOSS_JR_ELEC_APP); 
			PlaySound(this, SE_BOSS_JR_DAMAGE_ELEC); 

			float boltsize = (leader-14.0)/2;
			float boltpos = -boltsize - 14.0;

			bolt.spawn("Wm_jr_electricline", 0, &(Vec){pos.x, pos.y + boltpos, pos.z}, &(S16Vec){0,0,0}, &(Vec){1.0, boltsize/36.0, 1.0});
			break;
		case 3:
			this->Lightning.removeFromList();
			charge.spawn("Wm_mr_electricshock_biri02_s", 0, &pos, &(S16Vec){0,0,0}, &(Vec){1.5, 1.5, 1.5});
			break;
		case 4:
			charge.spawn("Wm_mr_electricshock_biri02_s", 0, &pos, &(S16Vec){0,0,0}, &(Vec){1.5, 1.5, 1.5});
			break;
		case 5:
			if (stationary) { doStateChange(&StateID_Wait); }
			else 			{ doStateChange(&StateID_Follow); }
			break;
		default:
			charge.spawn("Wm_mr_electricshock_biri02_s", 0, &pos, &(S16Vec){0,0,0}, &(Vec){1.5, 1.5, 1.5});
			break;
	}

	if(this->anm.isAnimationDone() && this->anm.getCurrentFrame() != 0.0) {
		if (timer == 2 && usingEvents) {
			if (dFlagMgr_c::instance->flags & eventFlag) {
			} else {
				this->Lightning.removeFromList();
				doStateChange(&StateID_Wait);
			}
		} else {
			timer++;
			if (timer == 2) { 
				this->bindAnimChr_and_setUpdateRate("thundershoot", 1, 0.0, 1.0); 
				lightningStrike();
			} else if (timer == 3) {
				this->bindAnimChr_and_setUpdateRate("cloud_wait", 1, 0.0, 1.0);
			}
		}
		this->anm.setCurrentFrame(0.0);
	}
}

void dThunderCloud::endState_Lightning() {
	this->timer = 0;
}


// Thundercloud center = 0
// Thundercloud bottom = -12
// Thundercloud boltpos = -boltsize/2 - 14.0
// Thundercloud boltsize = (leader-14.0)/2
// Thundercloud effSize = 36.0 [*2]
// Thundercloud effScale = boltsize / effSize
// Thundercloud effPos = -boltsize/2 - 14.0



//
// processed\../src/makeYourOwnModelSprite.cpp
//

#include <common.h>
#include <game.h>
#include <g3dhax.h>


//////////////////////////////////////////////////////////
//
//	How it works:
//
//		1) Skip down to line 70 - read the comments along the way if you like
//		2) Change the stuff inside " " to be what you want.
//		3) Copy paste an entire 'case' section of code, and change the number to change the setting it uses
//		4) give it back to Tempus to compile in
//



// This is the class allocator, you don't need to touch this
class dMakeYourOwn : public dStageActor_c {
	// Let's give ourselves a few functions
	int onCreate();
	int onDelete();
	int onExecute();
	int onDraw();

	static dMakeYourOwn *build();

	// And a model and an anmChr
	mHeapAllocator_c allocator;
	m3d::mdl_c bodyModel;
	nw4r::g3d::ResFile resFile;
	m3d::anmChr_c chrAnimation;

	nw4r::g3d::ResMdl mdl;

	// Some variables to use
	int model;
	bool isAnimating;
	float size;
	float zOrder;
	bool customZ;

	void setupAnim(const char* name, float rate);
	void setupModel(const char* arcName, const char* brresName, const char* mdlName);
};

// This sets up how much space we have in memory
dMakeYourOwn *dMakeYourOwn::build() {
	void *buffer = AllocFromGameHeap1(sizeof(dMakeYourOwn));
	return new(buffer) dMakeYourOwn;
}


// Saves space when we do it like this
void dMakeYourOwn::setupAnim(const char* name, float rate) {
	if (isAnimating) {
		nw4r::g3d::ResAnmChr anmChr;

		anmChr = this->resFile.GetResAnmChr(name);
		this->chrAnimation.setup(this->mdl, anmChr, &this->allocator, 0);
		this->chrAnimation.bind(&this->bodyModel, anmChr, 1);
		this->bodyModel.bindAnim(&this->chrAnimation, 0.0);
		this->chrAnimation.setUpdateRate(rate);
	}
}

void dMakeYourOwn::setupModel(const char* arcName, const char* brresName, const char* mdlName) {
	this->resFile.data = getResource(arcName, brresName);
	this->mdl = this->resFile.GetResMdl(mdlName);

	bodyModel.setup(mdl, &allocator, 0x224, 1, 0);
}


// This gets run when the sprite spawns!
int dMakeYourOwn::onCreate() {

	// Settings for your sprite!

	this->model = this->settings & 0xFF; 						// Sets nubble 12 to choose the model you want
	this->isAnimating = this->settings & 0x100;					// Sets nybble 11 to a checkbox for whether or not the model has an anmChr to use
	this->size = (float)((this->settings >> 24) & 0xFF) / 4.0; 	// Sets nybbles 5-6 to size. Size equals value / 4.


	float zLevels[16] = {-6500.0, -5000.0, -4500.0, -2000.0, 
						 -1000.0, 300.0, 800.0, 1600.0, 
						  2000.0, 3600.0, 4000.0, 4500.0, 
						  6000.0, 6500.0, 7000.0, 7500.0 };

	this->zOrder = zLevels[(this->settings >> 16) & 0xF];

	this->customZ = (((this->settings >> 16) & 0xF) != 0);

	// Setup the models inside an allocator
	allocator.link(-1, GameHeaps[0], 0, 0x20);


	// Makes the code shorter and clearer to put these up here

	// A switch case, add extra models in here
	switch (this->model) {

		// TITLESCREEN STUFF
		// DEFAULT 

		case 0:		//Red ballon, bobs

			setupModel("arrow", "g3d/bre0.brres", "ballon_red"); 
			SetupTextures_Item(&bodyModel, 0);
			this->pos.z = -3300.0;

			setupAnim("anim00", 1.0); 

			break;	

		case 1:		//Green ballon, bobs

			setupModel("arrow", "g3d/bre1.brres", "ballon_green"); 
			SetupTextures_Item(&bodyModel, 0);
			this->pos.z = 3300.0;

			setupAnim("anim01", 1.0); 
			break;	
			
		case 2:		// Mario, using "wait" with mouth open

			setupModel("arrow", "g3d/bre2.brres", "mario_ts"); 
			SetupTextures_Player(&bodyModel, 0);
			this->pos.z = -3000.0;

			setupAnim("anim02", 1.0); 
			break;	
						
		case 3:		// Peach, custom anim, bobs

			setupModel("arrow", "g3d/bre3.brres", "peach_ts"); 
			SetupTextures_Enemy(&bodyModel, 0);
			this->pos.z = -3000.0;

			setupAnim("anim03", 1.0); 
			break;	

		case 4:		// Luigi with mouth open using "wait", bobs

			setupModel("arrow", "g3d/bre4.brres", "luigi_ts"); 
			SetupTextures_Player(&bodyModel, 0);
			this->pos.z = 3000.0;

			setupAnim("anim04", 1.0); 
			break;	
			
		case 5:	 // Yellow Toad with mouth open, does wait, bobs

			setupModel("arrow", "g3d/bre5.brres", "toady_ts"); 
			SetupTextures_Player(&bodyModel, 0);
			this->pos.z = 3000.0;

			setupAnim("anim05", 1.0); 
			break;	

		case 6:		// Blue Toad with mouth open, bobs head and himself

			setupModel("arrow", "g3d/bre6.brres", "toadb_ts"); 
			SetupTextures_Player(&bodyModel, 0);
			this->pos.z = 3000.0;

			setupAnim("anim06", 1.0); 
			break;	
			
		// BOWSER BEAT TS
		
		case 7:		// Mario's clowncar, bobs, animates propeller

			setupModel("block_arrow", "g3d/bre7.brres", "clowncar_mario"); 
			SetupTextures_MapObj(&bodyModel, 0);
			this->pos.z = 3300.0;

			setupAnim("anim07", 1.0); 
			break;	
			
		case 8:		// Weegee clowncar, bobs, animates propeller, spins

			setupModel("block_arrow", "g3d/bre8.brres", "clowncar_luigi"); 
			SetupTextures_MapObj(&bodyModel, 0);
			this->pos.z = 3300.0;

			setupAnim("anim08", 2.0); 
			break;	
			
		case 9:		// Toad Yellow clowncar, bobs, animates propeller

			setupModel("block_arrow", "g3d/bre9.brres", "clowncar_toady"); 
			SetupTextures_MapObj(&bodyModel, 0);
			this->pos.z = 3300.0;

			setupAnim("anim09", 1.0); 
			break;	
			
		case 10:	// Toad Blue, bobs, animates propeller

			setupModel("block_arrow", "g3d/bre10.brres", "clowncar_toadb"); 
			SetupTextures_MapObj(&bodyModel, 0);
			this->pos.z = 3300.0;

			setupAnim("anim10", 1.0); 
			break;	
		
		case 11:	// Peach clowncar, bobs, animates propeller

			setupModel("block_arrow", "g3d/bre11.brres", "clowncar_peach"); 
			SetupTextures_MapObj(&bodyModel, 0);
			this->pos.z = 3300.0;

			setupAnim("anim11", 1.0); 
			break;	
	
		case 12:	// Mario in a clowncar, bobbing, with fist outstretched.
		
			setupModel("block_arrow", "g3d/bre12.brres", "mario_end"); 
			SetupTextures_Player(&bodyModel, 0);
			this->pos.z = 3000.0;

			setupAnim("anim12", 1.0); 
			break;	
			
		case 13:	// Weegee failing

			setupModel("block_arrow", "g3d/bre13.brres", "luigi_end"); 
			SetupTextures_Player(&bodyModel, 0);
			this->pos.z = 3000.0;

			setupAnim("anim13", 2.0); 
			break;	
			
		case 14:		// Toad Yellow, bobs head, bobs

			setupModel("block_arrow", "g3d/bre14.brres", "toady_end"); 
			SetupTextures_Player(&bodyModel, 0);
			this->pos.z = 3000.0;

			setupAnim("anim14", 1.0);
			break;	
			
		case 15:		// Blue Toad, bobs head, bobs

			setupModel("block_arrow", "g3d/bre15.brres", "toadb_end"); 
			SetupTextures_Player(&bodyModel, 0);
			this->pos.z = 3000.0;

			setupAnim("anim15", 1.0); 
			break;	
			
		case 16:		// Peach laughing, bobbing

			setupModel("block_arrow", "g3d/bre16.brres", "peach_end"); 
			SetupTextures_Enemy(&bodyModel, 0);
			this->pos.z = 3000.0;

			setupAnim("anim16", 1.0); 
			break;	
			
	//PERFECT FILE TS
			
		case 17:		// This is the peach castle backdrop

			setupModel("arrow", "g3d/bre17.brres", "ground_perfect"); 
			SetupTextures_Map(&bodyModel, 0);
			SetupTextures_MapObj(&bodyModel, 0);
			this->pos.z = 3300.0;

			setupAnim("anim17", 1.0); 
			break;	
			
		case 18:		// Mario very small, looking up.

			setupModel("arrow", "g3d/bre18.brres", "mario_perfect"); 
			SetupTextures_Player(&bodyModel, 0);
			this->pos.z = 3300.0;

			setupAnim("anim18", 1.0);
			break;	
			
		case 19:		// Weegee very small, looking up.

			setupModel("arrow", "g3d/bre19.brres", "luigi_perfect"); 
			SetupTextures_Player(&bodyModel, 0);
			this->pos.z = 3000.0;

			setupAnim("anim19", 1.0); 
			break;	
			
		case 20:		// Yellow Toad, very small, looking up.

			setupModel("arrow", "g3d/bre16.brres", "toady_perfect"); 
			SetupTextures_Player(&bodyModel, 0);
			this->pos.z = 3000.0;

			setupAnim("anim20", 1.0); 
			break;	
			
		case 21:		// Blue Toad, very small, looking up.

			setupModel("arrow", "g3d/bre16.brres", "toadb_perfect"); 
			SetupTextures_Player(&bodyModel, 0);
			this->pos.z = 3000.0;

			setupAnim("anim21", 1.0); 
			break;	
			
		case 22:		// I don't think this is used, actually :|

			setupModel("arrow", "g3d/bre22.brres", "peach_perfect"); 
			SetupTextures_Enemy(&bodyModel, 0);
			this->pos.z = 3000.0;

			setupAnim("anim22", 1.0); 
			break;	
			
		case 23:		// I don't think this is used, actually :|

			setupModel("arrow", "g3d/bre23.brres", "backdrop"); 
			SetupTextures_Map(&bodyModel, 0);
			this->pos.z = 3000.0;

			setupAnim("anim23", 1.0); 
			break;	
			
	// A level thing
			
		case 24:		// Small cloud, bobs up and down

			setupModel("arrow", "g3d/bre24.brres", "cloud"); 
			SetupTextures_Item(&bodyModel, 0);
			this->pos.z = -3300.0;

			setupAnim("anim24", 1.0); 

			break;	
	
	// Here begins the ending crap 
	
		case 25:		// Ship fallen, with broken propellers and cannons.

			setupModel("cage_boss_koopa", "g3d/ShipFallen.brres", "KoopaShip"); 
			SetupTextures_MapObj(&bodyModel, 0);
			this->pos.z = 3000.0;

			setupAnim("animation", 1.0);

			break;	
			
		case 26:		// A tree. From the ghost bg.

			setupModel("cage_boss_koopa", "g3d/tree_end.brres", "tree"); 
			SetupTextures_Map(&bodyModel, 0);
			this->pos.z = 3000.0;

			setupAnim("animation", 1.0); 
			
			break;	
			
		case 27:		// Bowser, laying down, eyes closed. Medic? Medic!

			setupModel("cage_boss_koopa", "g3d/bowser_dead.brres", "koopa"); 
			SetupTextures_Boss(&bodyModel, 0);
			this->pos.z = 3000.0;

			setupAnim("animation", 1.0);

			break;	
		
		case 28:		// A car. The animation has it tilted slightly. It's a bit darker than usual.

			setupModel("cage_boss_koopa", "g3d/clown_car_end.brres", "car"); 
			SetupTextures_MapObj(&bodyModel, 0);
			this->pos.z = 3000.0;

			setupAnim("animation", 1.0); 

			break;	
	//CREDITS SHIT
		case 29:		// Mario's clowncar, bobs, animates propeller

			setupModel("kameck_princess", "g3d/bre29.brres", "clowncar_mario"); 
			SetupTextures_MapObj(&bodyModel, 0);
			this->pos.z = 0.0;

			setupAnim("anim29", 1.0); 
			break;	
			
		case 30:		// Weegee clowncar, bobs, animates propeller

			setupModel("kameck_princess", "g3d/bre30.brres", "clowncar_luigi"); 
			SetupTextures_MapObj(&bodyModel, 0);
			this->pos.z = 0.0;

			setupAnim("anim30", 1.0); 
			break;	
			
		case 31:		// Toad Yellow clowncar, bobs, animates propeller

			setupModel("kameck_princess", "g3d/bre31.brres", "clowncar_toady"); 
			SetupTextures_MapObj(&bodyModel, 0);
			this->pos.z = 0.0;

			setupAnim("anim31", 1.0); 
			break;	
			
		case 32:	// Toad Blue, bobs, animates propeller

			setupModel("kameck_princess", "g3d/bre32.brres", "clowncar_toadb"); 
			SetupTextures_MapObj(&bodyModel, 0);
			this->pos.z = 0.0;

			setupAnim("anim32", 1.0); 
			break;	
		
		case 33:	// Peach clowncar, bobs, animates propeller

			setupModel("kameck_princess", "g3d/bre33.brres", "clowncar_peach"); 
			SetupTextures_MapObj(&bodyModel, 0);
			this->pos.z = 0.0;

			setupAnim("anim33", 1.0); 
			break;	
	
		case 34:	// Mario in a clowncar, bobbing, with fist outstretched.
		
			setupModel("kameck_princess", "g3d/bre34.brres", "mario_end"); 
			SetupTextures_Player(&bodyModel, 0);
			this->pos.z = 0;

			setupAnim("anim34", 1.0); 
			break;	
			
		case 35:	// Weegee 

			setupModel("kameck_princess", "g3d/bre35.brres", "luigi_end"); 
			SetupTextures_Player(&bodyModel, 0);
			this->pos.z = 0.0;

			setupAnim("anim35", 1.0); 
			break;	
			
		case 36:		// Toad Yellow, bobs head, bobs

			setupModel("kameck_princess", "g3d/bre36.brres", "toady_end"); 
			SetupTextures_Player(&bodyModel, 0);
			this->pos.z = 0.0;

			setupAnim("anim36", 1.0);
			break;	
			
		case 37:		// Blue Toad, bobs head, bobs

			setupModel("kameck_princess", "g3d/bre37.brres", "toadb_end"); 
			SetupTextures_Player(&bodyModel, 0);
			this->pos.z = 0.0;

			setupAnim("anim37", 1.0); 
			break;	
			
		case 38:		// Peach laughing, bobbing

			setupModel("kameck_princess", "g3d/bre38.brres", "peach_end"); 
			SetupTextures_Enemy(&bodyModel, 0);
			this->pos.z = 0.0;

			setupAnim("anim38", 1.0); 
			break;	
			
		case 39:		// PC Backdrop again

			setupModel("CreditsBG", "g3d/dupa.brres", "ground_perfect"); 
			SetupTextures_Map(&bodyModel, 0);
			this->pos.z = 0.0;

			setupAnim("anim38", 1.0); 
			break;	

		case 40:		// Chestnut Canopy

			setupModel("chestnut", "g3d/canopy.brres", "canopy"); 
			SetupTextures_MapObj(&bodyModel, 0);
			this->pos.z = 0.0;

			// setupAnim("anim38", 1.0); 
			break;	

		case 41:		// Chestnut Canopy

			setupModel("chestnut", "g3d/canopy_1.brres", "canopy_1"); 
			SetupTextures_MapObj(&bodyModel, 0);
			this->pos.z = 0.0;

			// setupAnim("anim38", 1.0); 
			break;	

		case 42:		// Chestnut Canopy

			setupModel("chestnut", "g3d/canopy_2.brres", "canopy_2"); 
			SetupTextures_MapObj(&bodyModel, 0);
			this->pos.z = 0.0;

			// setupAnim("anim38", 1.0); 
			break;	
		
		case 43:		// BallonR

			setupModel("OpeningScene", "g3d/ballon.brres", "ballon_red"); 
			SetupTextures_Item(&bodyModel, 0);
			this->pos.z = 0.0;

				setupAnim("anim", 1.0); 
			break;	
			
		case 44:		// BallonG

			setupModel("OpeningScene", "g3d/ballon2.brres", "ballon_green"); 
			SetupTextures_Item(&bodyModel, 0);
			this->pos.z = 0.0;

				setupAnim("anim", 1.0); 
			break;	
			
		case 45:		// Luigi Opening

			setupModel("OpeningScene", "g3d/weeg.brres", "weeg"); 
			SetupTextures_Player(&bodyModel, 0);
			this->pos.z = 0.0;

				setupAnim("anim", 1.0); 
			break;	
			
		case 46:		// Mario Opening

			setupModel("OpeningScene", "g3d/maleo.brres", "maleo"); 
			SetupTextures_Player(&bodyModel, 0);
			this->pos.z = 0.0;

				setupAnim("anim", 1.0); 
			break;	
			
		case 47:		// ToaB

			setupModel("OpeningScene", "g3d/todb.brres", "todb"); 
			SetupTextures_Player(&bodyModel, 0);
			this->pos.z = 0.0;

				setupAnim("anim", 1.0); 
			break;	

		case 48:		// ToaY

			setupModel("OpeningScene", "g3d/tody.brres", "tody"); 
			SetupTextures_Player(&bodyModel, 0);
			this->pos.z = 0.0;

				setupAnim("anim", 1.0); 
			break;	

		case 49:		// Chestnut Canopy

			setupModel("chestnut", "g3d/canopy_3.brres", "canopy_3"); 
			SetupTextures_MapObj(&bodyModel, 0);
			this->pos.z = 0.0;

			// setupAnim("anim38", 1.0); 
			break;	
	}

	allocator.unlink();

	if (size == 0.0) {	// If the person has the size nybble at zero, make it normal sized
		this->scale = (Vec){1.0,1.0,1.0};	
	}
	else {				// Else, use our size
		this->scale = (Vec){size,size,size};	
	}
		
	this->onExecute();
	return true;
}


// YOU'RE DONE, no need to do anything below here.


int dMakeYourOwn::onDelete() {
	return true;
}

int dMakeYourOwn::onExecute() {
	if (isAnimating) {
		bodyModel._vf1C();	// Advances the animation one update

		if(this->chrAnimation.isAnimationDone()) {
			this->chrAnimation.setCurrentFrame(0.0);	// Resets the animation when it's done
		}
	}

	return true;
}

int dMakeYourOwn::onDraw() {
	if (customZ) {
		matrix.translation(pos.x, pos.y, this->zOrder); }	// Set where to draw the model : -5500.0 is the official behind layer 2, while 5500.0 is in front of layer 0.
	else {
		matrix.translation(pos.x, pos.y, pos.z - 6500.0); }	// Set where to draw the model : -5500.0 is the official behind layer 2, while 5500.0 is in front of layer 0.

	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);	// Set how to rotate the drawn model 

	bodyModel.setDrawMatrix(matrix);	// Apply matrix
	bodyModel.setScale(&scale);			// Apply scale
	bodyModel.calcWorld(true);			// Do some shit

	bodyModel.scheduleForDrawing();		// Add it to the draw list for the game
	return true;
}

//
// processed\../src/challengeStar.cpp
//

#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>


extern "C" bool SpawnEffect(const char*, int, Vec*, S16Vec*, Vec*);

extern int GlobalStarsCollected;


class dChallengeStar : public dEn_c {
	int onCreate();
	int onExecute();
	int onDelete();
	int onDraw();

	mHeapAllocator_c allocator;
	m3d::mdl_c bodyModel;

	mEf::es2 effect;

	u64 eventFlag;
	s32 timer;
	bool collected;

	static dChallengeStar *build();

	void updateModelMatrices();
	void playerCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	void yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther);

	bool collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther);

};


void dChallengeStar::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther) { 

	if (collected) {
		this->Delete(1);
		return; }


	PlaySoundAsync(this, SE_OBJ_BROOM_KEY_SHOW);
	SpawnEffect("Wm_ob_greencoinkira_a", 0, &this->pos, &(S16Vec){0,0,0}, &(Vec){0.8, 0.8, 0.8});
	
	GlobalStarsCollected--;
	if (GlobalStarsCollected == 0) {
		dFlagMgr_c::instance->flags |= this->eventFlag;
	}
	
	collected = true;

	this->Delete(1);
}

void dChallengeStar::yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther) { this->playerCollision(apThis, apOther); }
bool dChallengeStar::collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther) {
	this->playerCollision(apThis, apOther);
	return true;
}
bool dChallengeStar::collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther) {
	this->playerCollision(apThis, apOther);
	return true;
}
bool dChallengeStar::collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther) {
	this->playerCollision(apThis, apOther);
	return true;
}
bool dChallengeStar::collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther) {
	this->playerCollision(apThis, apOther);
	return true;
}
bool dChallengeStar::collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther) {
	this->playerCollision(apThis, apOther);
	return true;
}


dChallengeStar *dChallengeStar::build() {
	void *buffer = AllocFromGameHeap1(sizeof(dChallengeStar));
	return new(buffer) dChallengeStar;
}


int dChallengeStar::onCreate() {
	
	collected = false;
	char die = this->settings & 0xF;
	if (GetSpecificPlayerActor(die) == 0) { this->Delete(1); return 2; }

	GlobalStarsCollected++;

	allocator.link(-1, GameHeaps[0], 0, 0x20);

	nw4r::g3d::ResFile rf(getResource("I_star", "g3d/silver_star.brres"));
	bodyModel.setup(rf.GetResMdl("I_star"), &allocator, 0x224, 1, 0);
	SetupTextures_Map(&bodyModel, 0);

	allocator.unlink();
	
	ActivePhysics::Info HitMeBaby;
	HitMeBaby.xDistToCenter = 0.0;
	HitMeBaby.yDistToCenter = 3.0;
	HitMeBaby.xDistToEdge = 6.0;
	HitMeBaby.yDistToEdge = 6.0;
	HitMeBaby.category1 = 0x5;
	HitMeBaby.category2 = 0x0;
	HitMeBaby.bitfield1 = 0x4F;
	HitMeBaby.bitfield2 = 0x200;
	HitMeBaby.unkShort1C = 0;
	HitMeBaby.callback = &dEn_c::collisionCallback;

	this->aPhysics.initWithStruct(this, &HitMeBaby);
	this->aPhysics.addToList();

	char eventNum	= (this->settings >> 24)	& 0xFF;
	this->eventFlag = (u64)1 << (eventNum - 1);

	this->scale.x = 0.70;
	this->scale.y = 0.70;
	this->scale.z = 0.70;

	this->pos.x += 8.0;
	this->pos.y -= 14.0;
	this->pos.z = 3300.0;
	
	this->onExecute();
	return true;
}


int dChallengeStar::onDelete() {
	return true;
}

int dChallengeStar::onDraw() {
	bodyModel.scheduleForDrawing();
	return true;
}


void dChallengeStar::updateModelMatrices() {
	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	bodyModel.setDrawMatrix(matrix);
	bodyModel.setScale(&scale);
	bodyModel.calcWorld(false);
}

int dChallengeStar::onExecute() {
	updateModelMatrices();

	effect.spawn("Wm_ob_keyget02_kira", 0, &this->pos, &(S16Vec){0,0,0}, &(Vec){0.8, 0.8, 0.8});
	this->rot.y += 0x200;
	return true;
}


//
// processed\../src/bonusRoom.cpp
//

#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include <stage.h>

extern "C" void *MakeMarioEnterDemoMode();
extern "C" void *MakeMarioExitDemoMode();
extern "C" bool SpawnEffect(const char*, int, Vec*, S16Vec*, Vec*);
extern "C" void *StopBGMMusic();
extern "C" void *StartBGMMusic();
// extern "C" void *SoundRelatedClass;

// THIS IS A DUMB NAME
extern bool NoMichaelBuble;


// Controls the tempo for the songs.
// Lower numbers are faster, by a ratio factor. 

int Tempo[16] = {13,11,12,12,
				 8,8,7,15,
				 15,8,10,16,
				 12,10,8,8};

int Songs[16][4][16][3] = { 
 
	// 15

 	// First number is the block, Second is the note/sfx, Third is timing: 30 is one quarter note at 120 bpm, 0,0 ends the sequence

	// Some possibles
	// SMW Donut Plains
	// SMW Forest of Illusion
	// Paper Mario Battle Theme
	// Thwomp Volcano: PiT
	// SMRPG Forest?
	// SMRPG Nimbus Land?
	// Teehee Valley: Superstar Saga
	// SML Theme
	// Mario Sunshine Main Theme
	// Mario Kart DS Theme

	{ // Song 1 - Super Mario Bros Melody |***  |
		{{3,17,0},{3,17,2},{3,17,6},{1,13,10},{3,17,12},{5,20,16},{0,0,0}}, 
		{{1,13,0},{5,8,3},{3,5,6},{6,10,9},{7,12,11},{6,11,13},{6,10,14},{0,0,0}},
		{{1,13,0},{1,13,1},{1,13,3},{1,13,5},{2,15,6},{3,17,8},{1,13,9},{7,10,11},{6,8,12},{0,0,0}},
		{{1,13,0},{1,13,1},{1,13,3},{1,13,5},{2,15,6},{3,17,7},{0,0,0}}
	},

	{ // Song 2 - Super Mario Bros. Underground |**** |
		{{7,12,0},{7,24,2},{5,9,4},{5,21,6},{6,10,8},{6,22,10},{0,0,0}},
		{{3,5,0},{3,17,2},{1,2,4},{1,14,6},{2,3,8},{2,15,10},{0,0,0}},
		{{2,15,0},{1,14,1},{1,13,2},{7,12,3},{2,15,5},{1,14,7},{5,8,9},{4,7,11},{1,13,13},{0,0,0}},
		{{7,12,0},{4,18,1},{3,17,2},{2,16,3},{6,22,4},{5,21,5},{4,20,6},{2,15,10},{6,11,12},{6,10,14},{5,9,16},{5,8,18},{0,0,0}}		
	},

	{ // Song 3 - SMB Starman |**   |
		{{4,6,0},{4,6,2},{4,6,4},{2,3,6},{4,6,7},{4,6,9},{2,3,11},{4,6,12},{2,3,13},{4,6,14},{0,0,0}},
		{{3,5,0},{3,5,2},{3,5,4},{1,1,6},{3,5,7},{3,5,9},{1,1,11},{3,5,12},{1,1,13},{3,5,14},{0,0,0}},
		{{4,18,0},{4,18,2},{4,18,4},{2,15,6},{4,18,7},{4,18,9},{2,15,11},{4,18,12},{2,15,13},{4,18,14},{0,0,0}},
		{{3,17,0},{3,17,2},{3,17,4},{1,13,6},{3,17,7},{3,17,9},{1,13,11},{3,17,12},{1,13,13},{3,17,14},{0,0,0}}
	},

	{ // Song 4 - SMB3 Clouds |*    |
		{{1,8,0},{6,20,4},{1,8,12},{2,9,16},{7,21,20},{0,0,0}},
		{{6,23,0},{5,20,4},{1,8,8},{3,17,16},{4,18,20},{1,14,24},{6,11,32},{0,0,0}},
		{{1,8,0},{6,20,4},{1,8,12},{2,9,16},{7,21,20},{0,0,0}},
		{{6,23,0},{5,20,4},{1,8,8},{3,17,16},{4,18,20},{1,14,24},{6,11,32},{0,0,0}}
	},

	{ // Song 5 - SMB3 Hammer Bros |*****|
		{{2,16,0},{3,17,2},{2,16,4},{3,17,6},{2,15,8},{1,13,10},{6,10,12},{1,13,14},{0,0,0}},
		{{2,16,0},{3,17,2},{2,16,4},{3,17,6},{2,15,8},{1,13,10},{6,10,12},{1,13,14},{1,13,18},{3,16,20},{2,15,21},{1,13,22},{6,10,26},{1,13,28},{0,0,0}},
		{{4,17,0},{4,17,2},{5,19,4},{6,20,6},{7,21,8},{4,17,10},{4,17,14},{3,15,16},{3,15,18},{2,14,20},{3,15,22},{0,0,0}},
		{{2,16,0},{3,17,2},{2,16,4},{3,17,6},{2,15,8},{1,13,10},{6,10,12},{1,13,14},{5,17,18},{5,17,20},{5,17,22},{5,17,24},{5,17,26},{5,17,28},{5,17,30},{0,0,0}}
	},

	{ // Song 6 - SMB3 Underwater |***  |
		{{1,8,0},{3,13,2},{6,20,4},{5,19,6},{7,22,10},{0,0,0}},
		{{5,19,14},{6,20,16},{4,17,18},{3,13,20},{1,8,22},{2,10,26},{0,0,0}},
		{{1,7,0},{2,8,2},{4,13,4},{6,20,6},{5,19,8},{7,22,12},{0,0,0}},
		{{5,19,16},{6,20,18},{0,0,0}}

		// Fuckin' too hard
		// {{1,8,0},{3,13,2},{6,20,4},{5,19,6},{7,22,10},{5,19,14},{6,20,16},{4,17,18},{3,13,20},{1,8,22},{2,10,26},{0,0,0}},
		// {{1,7,0},{2,8,2},{4,13,4},{6,20,6},{5,19,8},{7,22,12},{5,19,16},{6,20,18},{0,0,0}},
		// {{2,10,0},{3,13,2},{6,22,4},{5,21,6},{7,24,10},{6,22,14},{4,20,16},{1,8,18},{6,22,20},{4,20,22},{0,0,0}},
		// {{1,8,0},{2,11,2},{5,20,4},{4,18,6},{5,20,10},{3,16,14},{6,22,16},{5,20,24},{0,0,0}}
	},

	{ // Song 7 - SMW Castle |*    |
		{{5,9,0},{4,6,8},{1,1,12},{2,3,16},{4,6,20},{0,0,0}},
		{{1,1,0},{4,6,4},{7,13,8},{5,9,12},{4,8,20},{0,0,0}},
		{{3,16,0},{2,15,4},{1,13,8},{2,15,12},{1,10,20},{0,0,0}},
		{{2,15,0},{1,13,4},{2,15,8},{3,16,12},{1,15,20},{0,0,0}}
	},

	{ // Song 8 - SMW Theme |**   |
		{{6,10,0},{4,6,4},{1,1,7},{2,3,8},{4,6,9},{4,6,11},{0,0,0}},
		{{2,3,0},{1,1,1},{4,6,3},{4,6,5},{7,13,7},{6,10,9},{5,8,12},{0,0,0}},
		{{6,10,0},{4,6,4},{1,1,7},{2,3,8},{4,6,9},{4,6,11},{5,8,15},{6,10,16},{4,6,17},{1,1,18},{2,3,20},{4,6,23},{0,0,0}},
		{{1,13,0},{2,15,2},{1,13,4},{2,15,6},{1,13,8},{1,1,11},{7,11,12},{6,10,13},{5,8,14},{4,6,16},{0,0,0}}
	},

	{ // Song 9 - Yoshi Story Theme |*    |
		{{5,17,0},{6,19,4},{5,17,5},{6,19,6},{5,17,8},{0,0,0}},
		{{3,14,0},{4,15,4},{3,14,5},{4,15,6},{3,14,8},{0,0,0}},
		{{5,17,0},{6,19,4},{5,17,5},{6,19,6},{5,17,8},{0,0,0}},
		{{4,15,0},{3,14,1},{4,15,2},{3,14,4},{2,12,8},{1,10,9},{2,12,10},{1,10,12},{0,0,0}}
	},

	{ // Song 10 - SM64 Peaches Castle |***  |
		{{4,8,0},{5,10,6},{4,8,10},{3,7,14},{4,8,16},{6,13,18},{7,17,20},{0,0,0}},
		{{3,6,0},{4,8,6},{3,6,10},{2,5,14},{3,6,16},{5,12,18},{6,15,20},{0,0,0}},
		{{2,5,0},{3,6,6},{2,5,10},{1,4,14},{2,5,16},{4,10,18},{5,13,20},{0,0,0}},
		{{4,10,2},{5,12,4},{6,13,6},{7,15,8},{6,13,10},{7,15,12},{4,10,14},{5,12,18},{4,10,20},{5,12,22},{6,13,24},{6,13,28},{6,13,34},{6,13,36},{6,13,38},{0,0,0}}
	},

	{ // Song 11 - SM64 Koopa Road |***  |
		{{3,6,0},{1,1,10},{3,6,12},{7,16,14},{6,15,16},{5,11,18},{3,6,20},{0,0,0}},
		{{3,6,0},{2,4,6},{3,6,10},{5,11,12},{4,9,14},{5,11,16},{0,0,0}},
		{{3,6,0},{1,1,10},{3,6,12},{7,16,14},{6,15,16},{5,11,18},{3,6,20},{0,0,0}},
		{{1,1,0},{3,6,2},{7,18,4},{6,16,6},{5,15,10},{4,13,20},{5,15,22},{0,0,0}}
	},

	{ // Song 12 - Frappe Snowland |***  |
		{{1,14,2},{2,15,3},{3,17,4},{6,22,5},{3,17,7},{0,0,0}},
		{{2,15,0},{1,14,2},{2,15,4},{1,14,5},{6,10,7},{3,5,9},{4,7,10},{3,5,11},{0,0,0}},
		{{1,14,2},{2,15,3},{3,17,4},{6,22,5},{3,17,7},{0,0,0}},
		{{2,15,0},{1,14,2},{2,15,4},{1,14,5},{6,10,7},{6,10,9},{7,12,10},{6,10,11},{0,0,0}}
	},

	{ // Song 13 - Ghost Luigi Mansion theme |**** |
		{{7,17,2},{7,17,4},{7,17,6},{7,17,8},{4,13,12},{7,17,14},{6,16,16},{3,12,20},{0,0,0}},
		{{5,15,2},{5,15,4},{5,15,6},{5,15,8},{3,12,12},{5,15,14},{4,13,16},{2,11,22},{3,12,24},{1,5,28},{0,0,0}},
		{{7,17,2},{7,17,4},{7,17,6},{7,17,8},{4,13,12},{7,17,14},{6,16,16},{3,12,20},{0,0,0}},
		{{5,15,2},{5,15,4},{5,15,6},{5,15,8},{3,12,12},{5,15,14},{7,17,16},{5,15,18},{4,13,20},{3,12,22},{1,10,24},{0,0,0}}
	},

	{ // Song 14 - Desert |**   |
		{{6,9,0},{4,6,4},{3,5,12},{3,5,16},{4,6,18},{0,0,0}},
		{{6,9,0},{4,6,4},{3,5,12},{3,5,16},{4,6,18},{0,0,0}},
		{{4,13,0},{3,12,20},{5,14,22},{6,17,24},{5,14,26},{4,13,32},{0,0,0}},
		{{1,9,0},{2,11,2},{3,12,4},{5,14,8},{4,13,12},{0,0,0}}
	},

	{ // Song 15 - Gusty Garden Galaxy |*****|
		{{5,14,0},{4,13,10},{6,16,12},{5,14,14},{3,9,16},{1,6,26},{2,7,28},{3,9,30},{3,9,32},{2,7,38},{0,0,0}},
		{{6,16,0},{5,15,10},{7,18,12},{6,16,14},{4,14,16},{3,13,22},{2,11,26},{3,13,30},{2,11,32},{1,9,38},{0,0,0}},
		{{6,21,0},{5,20,10},{7,23,12},{6,21,14},{4,19,16},{3,18,22},{4,19,32},{3,18,42},{6,21,44},{4,19,46},{3,18,48},{1,16,54},{0,0,0}},
		{{5,18,0},{4,17,10},{6,19,12},{5,18,14},{4,16,16},{3,15,22},{5,18,26},{4,16,30},{2,14,32},{1,13,38},{2,14,44},{4,16,48},{0,0,0}}
	},

	{ // Song 16 - Overworld or Cave
		{{0,0,0}},
		{{0,0,0}},
		{{0,0,0}},
		{{0,0,0}}
	}
};

const char* Prizes[10][4] = { 
	{ "I_kinoko", 		"g3d/I_kinoko.brres", 			"I_kinoko", 			"wait2" },
	{ "I_fireflower", 	"g3d/I_fireflower.brres", 		"I_fireflower", 		"wait2" },
	{ "I_propeller", 	"g3d/I_propeller.brres",	 	"I_propeller_model", 	"wait2" },
	{ "I_iceflower", 	"g3d/I_iceflower.brres", 		"I_iceflower", 			"wait2" },
	{ "I_penguin", 		"g3d/I_penguin.brres", 			"I_penguin", 			"wait2" },
	{ "I_kinoko_bundle","g3d/I_mini_kinoko.brres", 		"I_mini_kinoko", 		"wait2" },
	{ "I_star", 		"g3d/I_star.brres", 			"I_star", 				"wait2" },
	{ "I_hammer", 		"g3d/I_fireflower.brres", 		"I_fireflower", 			"wait2" },
	{ "I_kinoko_bundle","g3d/I_life_kinoko.brres", 		"I_life_kinoko", 		"wait2" },
	{ "obj_coin", 		"g3d/obj_coin.brres", 			"obj_coin", 			"wait2" }
};

const char* Notes[24] = {
	"sfx/3C",
	"sfx/3C#",
	"sfx/3D",
	"sfx/3D#",
	"sfx/3E",
	"sfx/3F",
	"sfx/3F#",
	"sfx/3G",
	"sfx/3G#",
	"sfx/3A",
	"sfx/3A#",
	"sfx/3B",
	"sfx/4C",
	"sfx/4C#",
	"sfx/4D",
	"sfx/4D#",
	"sfx/4E",
	"sfx/4F",
	"sfx/4F#",
	"sfx/4G",
	"sfx/4G#",
	"sfx/4A",
	"sfx/4A#",
	"sfx/4B"
};

const char* SAarcNameList [] = {
	"obj_coin",
	"I_hammer",
	"I_star",
	"block_light",
	"block_light_color",
	"I_kinoko_bundle",
	NULL	
};



/*****************************************************************************/
// Playing frickin' sounds

extern "C" void PlaySoundWithFunctionB4(void *spc, nw4r::snd::SoundHandle *handle, int id, int unk);
static nw4r::snd::StrmSoundHandle handle;

u8 hijackMusicWithSongName(const char *songName, int themeID, bool hasFast, int channelCount, int trackCount, int *wantRealStreamID);

void BonusMusicPlayer(int id) {
	if (handle.Exists())
		handle.Stop(0);

	int sID;
	hijackMusicWithSongName(Notes[id], -1, false, 2, 1, &sID);
	PlaySoundWithFunctionB4(SoundRelatedClass, &handle, sID, 1);
}



/*****************************************************************************/
// The Prize Model

class dSongPrize: public dEn_c {
public:
	int onCreate();
	int onDelete();
	int onExecute();
	int beforeDraw();
	int onDraw();

	void doSpin();

	mHeapAllocator_c allocator;
	nw4r::g3d::ResFile resFile;
	m3d::mdl_c bodyModel;
	m3d::anmChr_c aw;

	int queue;
	int p;
	int timer;

	HermiteKey keysX[0x10];
	unsigned int Xkey_count;
	HermiteKey keysY[0x10];
	unsigned int Ykey_count;
	HermiteKey keysS[0x10];
	unsigned int Skey_count;

	USING_STATES(dSongPrize);
	DECLARE_STATE(Wait);
	DECLARE_STATE(Shrink);
	DECLARE_STATE(Spin);

	static dSongPrize *build();
};

CREATE_STATE(dSongPrize, Wait);
CREATE_STATE(dSongPrize, Shrink);
CREATE_STATE(dSongPrize, Spin);

dSongPrize *dSongPrize::build() {
	void *buffer = AllocFromGameHeap1(sizeof(dSongPrize));
	return new(buffer) dSongPrize;
}

int dSongPrize::onCreate() {

	// Settings
	queue = this->settings & 0xF;
	int prize = this->settings >> 16;
	scale = (Vec){ 3.0, 3.0, 3.0 };

	p = prize;


	// Model creation	
	allocator.link(-1, GameHeaps[0], 0, 0x20);

	resFile.data = getResource(Prizes[p][0], Prizes[p][1]);
	nw4r::g3d::ResMdl mdl = resFile.GetResMdl(Prizes[p][2]);
	bodyModel.setup(mdl, &allocator, 0x224, 1, 0);
	SetupTextures_Item(&bodyModel, 0); // 800B42B0


	// Animation Assignment
	nw4r::g3d::ResAnmChr anmChr = resFile.GetResAnmChr("wait2");
	aw.setup(mdl, anmChr, &allocator, 0);
	aw.bind(&bodyModel, anmChr, 1);
	bodyModel.bindAnim(&aw, 0.0);
	aw.setUpdateRate(1.0);
	allocator.unlink();


	// Change State
	doStateChange(&dSongPrize::StateID_Wait);

	return true;
}

int dSongPrize::onDelete() { 
	return true; 
}

int dSongPrize::onExecute() {
	acState.execute();
	return true;
}

int dSongPrize::beforeDraw() { return 1; }
int dSongPrize::onDraw() {

	if (p == 9) {
		matrix.translation(pos.x, pos.y + (8.0 * scale.y), pos.z);
	}
	else {
		matrix.translation(pos.x, pos.y, pos.z);
	}

	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	bodyModel.setDrawMatrix(matrix);
	bodyModel.setScale(&scale);

	bodyModel.calcWorld(false);


	bodyModel.scheduleForDrawing();
	bodyModel._vf1C();

	if(this->aw.isAnimationDone())
		this->aw.setCurrentFrame(0.0);

	return true;
}

void dSongPrize::beginState_Wait() {}
void dSongPrize::executeState_Wait() {}
void dSongPrize::endState_Wait() {}

void dSongPrize::beginState_Shrink() {
	this->timer = 0;

	Xkey_count = 2;
	Ykey_count = 2;
	Skey_count = 2;

	// /* keysX[i] = { frame, value, slope }; */
	keysX[0] = (HermiteKey){ 0.0, pos.x, 0.8 };
	keysY[0] = (HermiteKey){ 0.0, pos.y, 0.8 };
	keysS[0] = (HermiteKey){ 0.0, 3.0, -0.8 };

	keysX[1] = (HermiteKey){ 60.0, pos.x + (30.0 * queue) - 172.0, 1.0 };
	keysY[1] = (HermiteKey){ 60.0, pos.y + 64.0, 1.0 };
	keysS[1] = (HermiteKey){ 60.0, 1.0, 0.0 };	
}
void dSongPrize::executeState_Shrink() {
	float modX = GetHermiteCurveValue(timer, keysX, Xkey_count);
	float modY = GetHermiteCurveValue(timer, keysY, Ykey_count);
	//float modS = GetHermiteCurveValue(timer, keysS, Skey_count);
	float modS = 3.0f - (timer * (2.0f / 60.0f));

	pos = (Vec){ modX, modY, pos.z };
	scale = (Vec){ modS, modS, modS };

	if (timer == 60) { doStateChange(&StateID_Wait); }

	timer += 1;
}
void dSongPrize::endState_Shrink() {}

void dSongPrize::beginState_Spin() {
	this->timer = 0;

	Xkey_count = 2;	
	keysX[0] = (HermiteKey){ 0.0, 0.0, 0.0 };
	keysX[1] = (HermiteKey){ 20.0, 65535.0, 0.0 };
}
void dSongPrize::executeState_Spin() {
	float modX = GetHermiteCurveValue(timer, keysX, Xkey_count);
	rot.y = (int)modX;

	if (timer == 20) { SpawnEffect("Wm_ob_flagget", 0, &(Vec){pos.x-10.0, pos.y-2.0, pos.z-100.0}, &(S16Vec){0,0,0}, &(Vec){1.0, 1.0, 1.0}); }
	if (timer == 30) { doStateChange(&StateID_Wait); }

	timer += 1;	
}
void dSongPrize::endState_Spin() {}



class dSongBlock : public daEnBlockMain_c {
public:
	Physics::Info physicsInfo;

	int onCreate();
	int onDelete();
	int onExecute();
	int beforeDraw();
	int onDraw();

	mHeapAllocator_c allocator;
	nw4r::g3d::ResFile resFile;
	m3d::mdl_c bodyModel;
	m3d::mdl_c glowModel;
	m3d::mdl_c deadModel;
	m3d::mdl_c deadglowModel;

	StageActorLight light;
	mHeapAllocator_c allocatorB;

	int note;
	int isGlowing;
	int glowTimer;
	int isDead;
	int standAlone;
	int item;

	void calledWhenUpMoveExecutes();
	void calledWhenDownMoveExecutes();
	void blockWasHit(bool isDown);
	void glow();
	void unglow();

	USING_STATES(dSongBlock);
	DECLARE_STATE(Wait);

	static dSongBlock *build();
};

/*****************************************************************************/
// Sing Along

class dSingAlong : public dStageActor_c {
	public:
		static dSingAlong *instance;
		static dSingAlong *build();

		void RegisterNote(int note);
		void addPowerups();

		int onCreate();
		int onDelete();
		int onExecute();
		int onDraw();

		//int beforeExecute() { return true; }
		int afterExecute(int) { return true; }

		dSingAlong() : state(this, &StateID_Intro) { }

		mHeapAllocator_c allocator;

		dSongPrize *PrizeModel;
		dSongPrize *Pa;
		dSongPrize *Pb;
		dSongPrize *Pc;
		dSongPrize *Pd;

		dSongBlock *SBa;
		dSongBlock *SBb;
		dSongBlock *SBc;
		dSongBlock *SBd;
		dSongBlock *SBe;
		dSongBlock *SBf;
		dSongBlock *SBg;

		int song;
		int prize[4];
 		int chorus;
		int currentNote;
		int endNote;
		int timer;
		int counter;
		int Powerups[10];
		int isResponding;
		int success;

		dStateWrapper_c<dSingAlong> state;

		// Intro, Call, Response, Display Prize, Failure, Win, Collect Prizes
		USING_STATES(dSingAlong);
		DECLARE_STATE(Intro);
		DECLARE_STATE(Call);
		DECLARE_STATE(Response);
		DECLARE_STATE(Prize);
		DECLARE_STATE(Failure);
		DECLARE_STATE(Win);
		DECLARE_STATE(Mistake);
};

dSingAlong *dSingAlong::instance = 0;
dSingAlong *dSingAlong::build() {
	void *buffer = AllocFromGameHeap1(sizeof(dSingAlong));
	dSingAlong *c = new(buffer) dSingAlong;

	instance = c;
	return c;
}


/*****************************************************************************/
// Events
int dSingAlong::onCreate() {
	NoMichaelBuble = true;
	StageC4::instance->_1D = 1; // enable no-pause

	// Load in the settings
	this->song = this->settings & 0xF;
	this->prize[0] = (this->settings >> 28) & 0xF;
	this->prize[1] = (this->settings >> 24) & 0xF;
	this->prize[2] = (this->settings >> 20) & 0xF;
	this->prize[3] = (this->settings >> 16) & 0xF;
	this->chorus = -1;
	this->currentNote = 0;
	this->success = 0;

	this->Powerups[0] = 0; // Mushroom
	this->Powerups[1] = 0; // Fireflower
	this->Powerups[2] = 0; // Propeller
	this->Powerups[3] = 0; // Iceflower
	this->Powerups[4] = 0; // Penguin
	this->Powerups[5] = 0; // MiniShroom
	this->Powerups[6] = 0; // Starman
	this->Powerups[7] = 0; // Hammer
	this->Powerups[8] = 0; // 1-ups
	this->Powerups[9] = 0; // Coins

	// Create and prepare the blocks
	S16Vec rot = (S16Vec){0,0,0};
	float x = pos.x;
	float y = pos.y - 40.0;
	float z = pos.z;

	SBa = (dSongBlock*)create(WM_KILLER, 1, &(Vec){x-96.0+(32.0*0), y, z}, &rot, 0);
	SBb = (dSongBlock*)create(WM_KILLER, 2, &(Vec){x-96.0+(32.0*1), y, z}, &rot, 0);
	SBc = (dSongBlock*)create(WM_KILLER, 3, &(Vec){x-96.0+(32.0*2), y, z}, &rot, 0);
	SBd = (dSongBlock*)create(WM_KILLER, 4, &(Vec){x-96.0+(32.0*3), y, z}, &rot, 0);
	SBe = (dSongBlock*)create(WM_KILLER, 5, &(Vec){x-96.0+(32.0*4), y, z}, &rot, 0);
	SBf = (dSongBlock*)create(WM_KILLER, 6, &(Vec){x-96.0+(32.0*5), y, z}, &rot, 0);
	SBg = (dSongBlock*)create(WM_KILLER, 7, &(Vec){x-96.0+(32.0*6), y, z}, &rot, 0);

	// // Trigger the intro state
	state.setState(&StateID_Intro);
	isResponding = 0;

	return true;
}

int dSingAlong::onExecute() {
	state.execute();

	return true;
}

int dSingAlong::onDraw() { return true; }

int dSingAlong::onDelete() { 
	instance = 0;
	return 1;
}

/*****************************************************************************/
// Register a Note being played by the players
void dSingAlong::RegisterNote(int note) {
	nw4r::snd::SoundHandle handle;

	if (isResponding == 1) {

		if (note == Songs[song][chorus][currentNote][0]) {
			BonusMusicPlayer(Songs[song][chorus][currentNote][1]-1);
			// MapSoundPlayer(SoundRelatedClass, Notes[Songs[song][chorus][currentNote][1]-1], 1);
			currentNote += 1;
		}
		else {
			PlaySoundWithFunctionB4(SoundRelatedClass, &handle, SE_MG_CMN_WIN_CLOSE, 1);
			isResponding = 0;
			state.setState(&StateID_Mistake);
		}
	}
}

/*****************************************************************************/
// Intro
CREATE_STATE(dSingAlong, Intro);

void dSingAlong::beginState_Intro() {}
void dSingAlong::endState_Intro() {}
void dSingAlong::executeState_Intro() {
	MakeMarioEnterDemoMode();
	state.setState(&StateID_Prize);
}

//*****************************************************************************/
// Prize
CREATE_STATE(dSingAlong, Prize);

void dSingAlong::beginState_Prize() {
	this->timer = 120;
}
void dSingAlong::executeState_Prize() {

	if ((timer == 120) && (chorus >= 0)) { // Play a nice success sound, and wait a second
		nw4r::snd::SoundHandle handle;
		PlaySoundWithFunctionB4(SoundRelatedClass, &handle, SE_MG_IH_PAIR_OK, 1);
		//PlaySound(this, SE_MG_IH_PAIR_OK); // SE_MG_IH_NICE or SE_MG_UH_NICE
		
		int p;
		p = prize[chorus];
		this->Powerups[p] += 1;
	}

	if ((timer == 60) && (chorus >= 0)) {
		if (chorus == 0) { Pa = PrizeModel; }
		if (chorus == 1) { Pb = PrizeModel; }
		if (chorus == 2) { Pc = PrizeModel; }
		if (chorus == 3) { Pd = PrizeModel; }

		PrizeModel->doStateChange(&dSongPrize::StateID_Shrink); 
	}

	if (timer == 0) {
		chorus += 1;

		if (chorus == 4) { 
			if (success != 4)
				state.setState(&StateID_Failure);
			else
				state.setState(&StateID_Win);
			return; 
		}

		SpawnEffect("Wm_en_blockcloud", 0, &(Vec){pos.x, pos.y+32.0, pos.z+500.0}, &(S16Vec){0,0,0}, &(Vec){1.5, 1.5, 1.5});
		nw4r::snd::SoundHandle handle;
		PlaySoundWithFunctionB4(SoundRelatedClass, &handle, SE_OBJ_ITEM_APPEAR, 1);
		//PlaySound(this, SE_OBJ_ITEM_APPEAR); // SE_OBJ_GOOD_ITEM_APPEAR

		PrizeModel = (dSongPrize*)create(WM_SINKSHIP, chorus + (prize[chorus] << 16), &pos, &rot, 0);
	}

	if (timer == -90) {
		state.setState(&StateID_Call);
	}

	timer -= 1;
}
void dSingAlong::endState_Prize() {}


//*****************************************************************************/
// Call
CREATE_STATE(dSingAlong, Call);

void dSingAlong::beginState_Call() { 
	timer = 0;
	currentNote = 0;
}
void dSingAlong::executeState_Call() {
	if (timer == (Songs[song][chorus][currentNote][2] * Tempo[song])) {

		BonusMusicPlayer(Songs[song][chorus][currentNote][1]-1);

		Vec effPos;

		if      (Songs[song][chorus][currentNote][0] == 1) { SBa->glow(); effPos = SBa->pos; }
		else if (Songs[song][chorus][currentNote][0] == 2) { SBb->glow(); effPos = SBb->pos; }
		else if (Songs[song][chorus][currentNote][0] == 3) { SBc->glow(); effPos = SBc->pos; }
		else if (Songs[song][chorus][currentNote][0] == 4) { SBd->glow(); effPos = SBd->pos; }
		else if (Songs[song][chorus][currentNote][0] == 5) { SBe->glow(); effPos = SBe->pos; }
		else if (Songs[song][chorus][currentNote][0] == 6) { SBf->glow(); effPos = SBf->pos; }
		else if (Songs[song][chorus][currentNote][0] == 7) { SBg->glow(); effPos = SBg->pos; }

		SpawnEffect("Wm_en_vshit_ring", 0, &(Vec){effPos.x, effPos.y+8.0, effPos.z-100.0}, &(S16Vec){0,0,0}, &(Vec){1.0, 1.0, 1.0});

		currentNote += 1;

		if (Songs[song][chorus][currentNote][0] == 0) {
			state.setState(&StateID_Response);
		}
	}

	timer += 1;
}
void dSingAlong::endState_Call() {
	MakeMarioExitDemoMode();
}


/*****************************************************************************/
// Response
CREATE_STATE(dSingAlong, Response);

void dSingAlong::beginState_Response() { 
	timer = 0;
	currentNote = 0;
	isResponding = 1;
}
void dSingAlong::executeState_Response() {
	if (Songs[song][chorus][currentNote][0] == 0) {
		isResponding = 0;
		MakeMarioEnterDemoMode();
		this->success = this->success + 1;
		state.setState(&StateID_Prize);
	}
}
void dSingAlong::endState_Response() {}


//*****************************************************************************/
// Prize
CREATE_STATE(dSingAlong, Mistake);

void dSingAlong::beginState_Mistake() {
	SBa->unglow(); 
	SBb->unglow(); 
	SBc->unglow(); 
	SBd->unglow(); 
	SBe->unglow(); 
	SBf->unglow(); 
	SBg->unglow(); 

	MakeMarioEnterDemoMode();
	this->timer = 120;
	chorus += 1;
}
void dSingAlong::executeState_Mistake() {

	if (timer == 60) {
		nw4r::snd::SoundHandle handle;
		PlaySoundWithFunctionB4(SoundRelatedClass, &handle, SE_MG_IH_NOPAIR_NG, 1);
		
		SpawnEffect("Wm_en_blockcloud", 0, &(Vec){pos.x, pos.y+32.0, pos.z+500.0}, &(S16Vec){0,0,0}, &(Vec){1.5, 1.5, 1.5});
		PrizeModel->Delete(1);
	}

	if (chorus >= 4 && timer == 15) {
		state.setState(&StateID_Failure);
		return;
	}

	if (timer == 0) {
		SpawnEffect("Wm_en_blockcloud", 0, &(Vec){pos.x, pos.y+32.0, pos.z+500.0}, &(S16Vec){0,0,0}, &(Vec){1.5, 1.5, 1.5});
		nw4r::snd::SoundHandle handle;
		PlaySoundWithFunctionB4(SoundRelatedClass, &handle, SE_OBJ_ITEM_APPEAR, 1);

		PrizeModel = (dSongPrize*)create(WM_SINKSHIP, chorus + (prize[chorus] << 16), &pos, &rot, 0);
	}

	if (timer == -90) {
		state.setState(&StateID_Call);
	}

	timer -= 1;
}
void dSingAlong::endState_Mistake() {

	if (chorus != 4) {
		SBa->isDead = 0;
		SBb->isDead = 0; 
		SBc->isDead = 0; 
		SBd->isDead = 0; 
		SBe->isDead = 0; 
		SBf->isDead = 0; 
		SBg->isDead = 0; 	
	}
}

/*****************************************************************************/
// Failure
CREATE_STATE(dSingAlong, Failure);

void dSingAlong::beginState_Failure() {
	this->timer = 0;

	MakeMarioEnterDemoMode();
}
void dSingAlong::executeState_Failure() {
	if (timer == 5) {
		if (success == 0) {
			for (int i = 0; i < 4; i++)
				if (dAcPy_c *player = dAcPy_c::findByID(i))
					player->setAnimePlayWithAnimID(dm_surprise);
		}
		else {
			for (int i = 0; i < 4; i++)
				if (dAcPy_c *player = dAcPy_c::findByID(i)) {
					player->setAnimePlayWithAnimID(dm_glad);
					player->setFlag(0x24);
				}   
		}
	}

	// Play a success/failure sound
	if (timer == 20) {
		StopBGMMusic();
		nw4r::snd::SoundHandle handle;

		if (success == 0)
			PlaySoundWithFunctionB4(SoundRelatedClass, &handle, STRM_BGM_MINIGAME_FANFARE_BAD, 1);
		else
			PlaySoundWithFunctionB4(SoundRelatedClass, &handle, STRM_BGM_MINIGAME_FANFARE_GOOD, 1);
	}

	// Delete the big powerup with a poof if it's fail
	// if (timer == 30*1) {
	// 	SpawnEffect("Wm_en_blockcloud", 0, &(Vec){pos.x, pos.y+32.0, pos.z+500.0}, &(S16Vec){0,0,0}, &(Vec){1.5, 1.5, 1.5});
	// 	nw4r::snd::SoundHandle handle;
	// 	PlaySoundWithFunctionB4(SoundRelatedClass, &handle, SE_MG_IH_NOPAIR_OK, 1);
	// 	PrizeModel->Delete(1);
	// 	if (chorus == 0)
	// 		timer += 140;
	// }

	// Make the banked powerups do a little dance/effect
	// Play a sound for each powerup gained
	if (timer == 30*2 || timer == 30*3 || timer == 30*4 || timer == 30*5) {
		dSongPrize *dancer = 0;
		if (timer == 30*2)
			dancer = Pa;
		else if (timer == 30*3)
			dancer = Pb;
		else if (timer == 30*4)
			dancer = Pc;
		else if (timer == 30*5)
			dancer = Pd;

		if (dancer) {
			dancer->doStateChange(&dSongPrize::StateID_Spin);
			nw4r::snd::SoundHandle handle;
			PlaySoundWithFunctionB4(SoundRelatedClass, &handle, SE_OBJ_ITEM_FROM_KINOPIO, 1);
		}
		else {
			timer += 29;
		}
	}

	// If victory, make mario do a little dance/sound
	if (timer == 30*7) {
		nw4r::snd::SoundHandle handle1, handle2, handle3, handle4;

		if (GetSpecificPlayerActor(0) != 0)
			PlaySoundWithFunctionB4(SoundRelatedClass, &handle1, (success == 0) ? SE_VOC_MA_CS_COURSE_MISS : SE_VOC_MA_CLEAR_MULTI, 1);
		if (GetSpecificPlayerActor(1) != 0)
			PlaySoundWithFunctionB4(SoundRelatedClass, &handle2, (success == 0) ? SE_VOC_LU_CS_COURSE_MISS : SE_VOC_LU_CLEAR_MULTI, 1);
		if (GetSpecificPlayerActor(2) != 0)
			PlaySoundWithFunctionB4(SoundRelatedClass, &handle3, (success == 0) ? SE_VOC_KO_CS_COURSE_MISS : SE_VOC_KO_CLEAR_MULTI, 1);
		if (GetSpecificPlayerActor(3) != 0)
			PlaySoundWithFunctionB4(SoundRelatedClass, &handle4, (success == 0) ? SE_VOC_KO2_CS_COURSE_MISS : SE_VOC_KO2_CLEAR_MULTI, 1);
	}


	// Add the powerups and exit the stage
	if (timer == 30*9) {
		this->addPowerups();
		ExitStage(WORLD_MAP, 0, BEAT_LEVEL, MARIO_WIPE);
	}
	timer += 1;
}
void dSingAlong::endState_Failure() {}


/*****************************************************************************/
// Win
CREATE_STATE(dSingAlong, Win);

void dSingAlong::beginState_Win() {
	this->timer = 0;
	MakeMarioEnterDemoMode();
}
void dSingAlong::executeState_Win() {
	if (timer == 5)
		for (int i = 0; i < 4; i++)
			if (dAcPy_c *player = dAcPy_c::findByID(i)) {
				player->setAnimePlayWithAnimID(dm_glad);
				player->setFlag(0x24);
			}

	// Play a success/failure sound
	if (timer == 30) {
		nw4r::snd::SoundHandle handle;
		PlaySoundWithFunctionB4(SoundRelatedClass, &handle, STRM_BGM_MINIGAME_FANFARE_GOOD, 1);
		StopBGMMusic();
	}

	// Make the banked powerups do a little dance/effect
	// Play a sound for each powerup gained
	if (timer == 30*3 || timer == 30*4 || timer == 30*5 || timer == 30*6) {
		dSongPrize *dancer = 0;
		if (timer == 30*3)
			dancer = Pa;
		else if (timer == 30*4)
			dancer = Pb;
		else if (timer == 30*5)
			dancer = Pc;
		else if (timer == 30*6)
			dancer = Pd;

		if (dancer) {
			dancer->doStateChange(&dSongPrize::StateID_Spin);
			nw4r::snd::SoundHandle handle;
			PlaySoundWithFunctionB4(SoundRelatedClass, &handle, SE_OBJ_ITEM_FROM_KINOPIO, 1);
		}
		else {
			timer += 29;
		}
	}

	// If victory, make mario do a little dance/sound
	if (timer == 30*8) {
		nw4r::snd::SoundHandle handle1, handle2, handle3, handle4;

		if (GetSpecificPlayerActor(0) != 0)
			PlaySoundWithFunctionB4(SoundRelatedClass, &handle1, SE_VOC_MA_CLEAR_MULTI, 1);
		if (GetSpecificPlayerActor(1) != 0)
			PlaySoundWithFunctionB4(SoundRelatedClass, &handle2, SE_VOC_LU_CLEAR_MULTI, 1);
		if (GetSpecificPlayerActor(2) != 0)
			PlaySoundWithFunctionB4(SoundRelatedClass, &handle3, SE_VOC_KO_CLEAR_MULTI, 1);
		if (GetSpecificPlayerActor(3) != 0)
			PlaySoundWithFunctionB4(SoundRelatedClass, &handle4, SE_VOC_KO2_CLEAR_MULTI, 1);

		SpawnEffect("Wm_ob_fireworks_y", 0, &(Vec){pos.x-80.0, pos.y+20.0, pos.z+500.0}, &(S16Vec){0,0,0}, &(Vec){0.75, 0.75, 0.75});
	}

	if (timer == (int)30*8.5) {
		SpawnEffect("Wm_ob_fireworks_b", 0, &(Vec){pos.x+108.0, pos.y+32.0, pos.z+500.0}, &(S16Vec){0,0,0}, &(Vec){0.75, 0.75, 0.75});
	}

	if (timer == 30*9) {
		SpawnEffect("Wm_ob_fireworks_g", 0, &(Vec){pos.x, pos.y+50.0, pos.z+500.0}, &(S16Vec){0,0,0}, &(Vec){0.75, 0.75, 0.75});
	}


	// Add the powerups and exit the stage
	if (timer == 30*11) {
		this->addPowerups();
		ExitStage(WORLD_MAP, 0, BEAT_LEVEL, MARIO_WIPE);
	}
	timer += 1;
}
void dSingAlong::endState_Win() {}


/*****************************************************************************/
// Add Powerups at the End of the Stage
void dSingAlong::addPowerups() {
	SaveFile *file = GetSaveFile();
	SaveBlock *block = file->GetBlock(file->header.current_file);

	for (int i = 0; i < 8; i++) { // Change this to 8 to support hammers
		block->powerups_available[i] = block->powerups_available[i] + this->Powerups[i];

		if (block->powerups_available[i] > 99) { block->powerups_available[i] = 99; }
	}

	for (int i = 0; i < 4; i++) { // Make sure all players get the reward!
		block->player_coins[i] = (this->Powerups[9] * 50) + block->player_coins[i];

		for (;block->player_coins[i] < 100; block->player_coins[i] - 100) {
			block->player_coins[i] = 1 + block->player_coins[i];
		}

		block->player_lives[i] = this->Powerups[8] + block->player_lives[i];
		if (block->player_lives[i] > 99) { block->player_lives[i] = 99; }
	}

	return;
}


/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
// Replaces: Nothing yet



CREATE_STATE(dSongBlock, Wait);


dSongBlock *dSongBlock::build() {
	void *buffer = AllocFromGameHeap1(sizeof(dSongBlock));
	return new(buffer) dSongBlock;
}


int dSongBlock::onCreate() {

	// Settings
	this->note = this->settings & 0xF;
	this->standAlone = (this->settings >> 4) & 0xF;
	this->item = (this->settings >> 16) & 0xFF;

	// Model creation	
	allocator.link(-1, GameHeaps[0], 0, 0x20);

	char modelName [24];
	sprintf(modelName, "g3d/block_light_%d.brres", this->note);

	this->resFile.data = getResource("block_light", modelName);
	nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("block_light");
	bodyModel.setup(mdl, &allocator, 0x224, 1, 0);
	SetupTextures_MapObj(&bodyModel, 0); // 800B42B0

	char glowName [25];
	sprintf(glowName, "g3d/block_light_g%d.brres", this->note);

	this->resFile.data = getResource("block_light", glowName);
	mdl = this->resFile.GetResMdl("block_light");
	glowModel.setup(mdl, &allocator, 0x224, 1, 0);
	SetupTextures_MapObj(&glowModel, 0); // 800B42B0



	// Dead Models
	this->resFile.data = getResource("block_light", "g3d/block_light_8.brres");
	mdl = this->resFile.GetResMdl("block_light");
	deadModel.setup(mdl, &allocator, 0x224, 1, 0);
	SetupTextures_MapObj(&deadModel, 0); // 800B42B0

	this->resFile.data = getResource("block_light", "g3d/block_light_g8.brres");
	mdl = this->resFile.GetResMdl("block_light");
	deadglowModel.setup(mdl, &allocator, 0x224, 1, 0);
	SetupTextures_MapObj(&deadglowModel, 0); // 800B42B0

	allocator.unlink();

	// Glow related
	isGlowing = 0;
	glowTimer = 0;
	isDead = 0;

	light.init(&allocatorB, 2);


	// Block Physics
	blockInit(pos.y);

	physicsInfo.x1 = -8;
	physicsInfo.y1 = 16;
	physicsInfo.x2 = 8;
	physicsInfo.y2 = 0;

	physicsInfo.otherCallback1 = &daEnBlockMain_c::OPhysicsCallback1;
	physicsInfo.otherCallback2 = &daEnBlockMain_c::OPhysicsCallback2;
	physicsInfo.otherCallback3 = &daEnBlockMain_c::OPhysicsCallback3;

	physics.setup(this, &physicsInfo, 3, currentLayerID);
	physics.flagsMaybe = 0x260;
	physics.callback1 = &daEnBlockMain_c::PhysicsCallback1;
	physics.callback2 = &daEnBlockMain_c::PhysicsCallback2;
	physics.callback3 = &daEnBlockMain_c::PhysicsCallback3;
	physics.addToList();

	// Change State
	doStateChange(&dSongBlock::StateID_Wait);

	return true;
}

int dSongBlock::onDelete() {
	physics.removeFromList();
	return true;
}

int dSongBlock::onExecute() {
	acState.execute();
	physics.update();
	blockUpdate();

	if (glowTimer > 0) { glowTimer--; }
	else { isGlowing = 0; }

	light.pos.x = pos.x;
	light.pos.y = pos.y+8.0;
	light.pos.z = pos.z;

	light.size = 226.0;
	light.update();

	// now check zone bounds based on state
	if (acState.getCurrentState()->isEqual(&StateID_Wait)) {
		checkZoneBoundaries(0);
	}

	return true;
}

int dSongBlock::beforeDraw() {
	light.draw();
	return dStageActor_c::beforeDraw();
}

int dSongBlock::onDraw() {
	matrix.translation(pos.x, pos.y+8.0, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	if (isDead == 0) {
		if (isGlowing == 0) {
			bodyModel.setDrawMatrix(matrix);
			bodyModel.setScale(&scale);
			bodyModel.calcWorld(false);

			bodyModel.scheduleForDrawing();
		}

		else {
			glowModel.setDrawMatrix(matrix);
			glowModel.setScale(&scale);
			glowModel.calcWorld(false);

			glowModel.scheduleForDrawing();
		}
	}
	else {
		if (isGlowing == 0) {
			deadModel.setDrawMatrix(matrix);
			deadModel.setScale(&scale);
			deadModel.calcWorld(false);

			deadModel.scheduleForDrawing();
		}

		else {
			deadglowModel.setDrawMatrix(matrix);
			deadglowModel.setScale(&scale);
			deadglowModel.calcWorld(false);

			deadglowModel.scheduleForDrawing();
		}
	}

	return true;
}

void dSongBlock::blockWasHit(bool isDown) {
	pos.y = initialY;

	SpawnEffect("Wm_en_vshit_ring", 0, &(Vec){pos.x, pos.y+8.0, pos.z-100.0}, &(S16Vec){0,0,0}, &(Vec){1.0, 1.0, 1.0});
	SpawnEffect("Wm_ob_keyget02_lighit", 0, &(Vec){pos.x, pos.y+8.0, pos.z-100.0}, &(S16Vec){0,0,0}, &(Vec){0.5, 0.5, 0.5});

	if (item > 0) {
		create(EN_ITEM, item, &(Vec){pos.x, pos.y, pos.z}, &rot, 0);
	}

	if (standAlone) {

		int play;
		// C, C#, D, D#, E, F, F#, G, G#, A, A#, B

		if 		(note == 0) { play = 12; }
		else if (note == 1) { play = 12; }
		else if (note == 2) { play = 14; }
		else if (note == 3) { play = 16; }
		else if (note == 4) { play = 17; }
		else if (note == 5) { play = 19; }
		else if (note == 6) { play = 21; }
		else if (note == 7) { play = 23; }
		else if (note == 8) { play = 0; }

		// nw4r::snd::SoundHandle handle;
		// PlaySoundWithFunctionB4(SoundRelatedClass, &handle, Notes[play], 1);
		BonusMusicPlayer(play);
	}
	else {
		dSingAlong::instance->RegisterNote(this->note);
	}

	physics.setup(this, &physicsInfo, 3, currentLayerID);
	physics.addToList();
	
	doStateChange(&StateID_Wait);
}

void dSongBlock::calledWhenUpMoveExecutes() {
	if (initialY >= pos.y)
		blockWasHit(false);
}

void dSongBlock::calledWhenDownMoveExecutes() {
	if (initialY <= pos.y)
		blockWasHit(true);
}

void dSongBlock::glow() {
	isGlowing = 1;
	isDead = 0;
	glowTimer = 15;
}

void dSongBlock::unglow() {
	isDead = 1;
	SpawnEffect("Wm_en_sanbohit_smk", 0, &(Vec){pos.x, pos.y+8.0, pos.z-100.0}, &(S16Vec){0,0,0}, &(Vec){0.4, 0.4, 0.4});
}

void dSongBlock::beginState_Wait() {}
void dSongBlock::endState_Wait() {}
void dSongBlock::executeState_Wait() {
	int result = blockResult();

	if (result == 0)
		return;

	if (result == 1) {
		glow();
		doStateChange(&daEnBlockMain_c::StateID_UpMove);
		anotherFlag = 2;
		isGroundPound = false;
	} else {
		glow();
		doStateChange(&daEnBlockMain_c::StateID_DownMove);
		anotherFlag = 1;
		isGroundPound = true;
	}
}



//
// processed\../src/pumpkinGoomba.cpp
//

#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>

const char* GParcNameList [] = {
	"kuribo",
	"pumpkin",
	"wing",
	NULL	
};

class dGoombaPie : public dEn_c {
	int onCreate();
	int onDelete();
	int onExecute();
	int onDraw();

	mHeapAllocator_c allocator;
	nw4r::g3d::ResFile resFile;
	m3d::mdl_c bodyModel;
	m3d::mdl_c burstModel;

	dStageActor_c *Goomber;
	u32 timer;
	bool isBursting;

	static dGoombaPie *build();

	void playerCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	void yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	void spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther);

	bool collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther);

	void _vf148();
	void _vf14C();
	bool CreateIceActors();
	void addScoreWhenHit(void *other);

	USING_STATES(dGoombaPie);
	DECLARE_STATE(Follow);
	DECLARE_STATE(Burst);
};

dGoombaPie *dGoombaPie::build() {
	void *buffer = AllocFromGameHeap1(sizeof(dGoombaPie));
	return new(buffer) dGoombaPie;
}

///////////////////////
// Externs and States
///////////////////////
	extern "C" void *EN_LandbarrelPlayerCollision(dEn_c* t, ActivePhysics *apThis, ActivePhysics *apOther);
	extern "C" int SmoothRotation(short* rot, u16 amt, int unk2);
	extern "C" char usedForDeterminingStatePress_or_playerCollision(dEn_c* t, ActivePhysics *apThis, ActivePhysics *apOther, int unk1);
	extern "C" bool SpawnEffect(const char*, int, Vec*, S16Vec*, Vec*);

	CREATE_STATE(dGoombaPie, Follow);
	CREATE_STATE(dGoombaPie, Burst);


////////////////////////
// Collision Functions
////////////////////////

	void pieCollisionCallback(ActivePhysics *one, ActivePhysics *two) {
		if (two->owner->name == EN_KURIBO) { return; }
		if (two->owner->name == EN_PATA_KURIBO) { return; }
		dEn_c::collisionCallback(one, two);
	}

	void dGoombaPie::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther) {

		char hitType;
		hitType = usedForDeterminingStatePress_or_playerCollision(this, apThis, apOther, 0);

		if(hitType == 1) {	// regular jump
			apOther->someFlagByte |= 2;
			doStateChange(&StateID_Burst);
		} 
		else if(hitType == 3) {	// spinning jump or whatever?
			apOther->someFlagByte |= 2;
			doStateChange(&StateID_Burst);
		} 
		else if(hitType == 0) {
			EN_LandbarrelPlayerCollision(this, apThis, apOther);
			if (this->pos.x > apOther->owner->pos.x) {
				this->direction = 1;
			}
			else {
				this->direction = 0;
			}
			doStateChange(&StateID_Burst);
		} 

		// fix multiple player collisions via megazig
		deathInfo.isDead = 0;
		this->flags_4FC |= (1<<(31-7));
		this->counter_504[apOther->owner->which_player] = 0;
	}

	void dGoombaPie::spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther) {}
	void dGoombaPie::yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther) { this->playerCollision(apThis, apOther); }
	bool dGoombaPie::collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther) { doStateChange(&StateID_Burst); return true; }
	bool dGoombaPie::collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther) { doStateChange(&StateID_Burst); return true; }
	bool dGoombaPie::collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther) { doStateChange(&StateID_Burst); return true; }
	bool dGoombaPie::collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther) { doStateChange(&StateID_Burst); return true; }
	bool dGoombaPie::collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther){ doStateChange(&StateID_Burst); return true; }
	bool dGoombaPie::collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther) { doStateChange(&StateID_Burst); return true; }
	bool dGoombaPie::collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther){ doStateChange(&StateID_Burst); return true; }
	bool dGoombaPie::collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther){ doStateChange(&StateID_DieSmoke); return true; }
	bool dGoombaPie::collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther) { doStateChange(&StateID_DieSmoke); return true; }

	// These handle the ice crap
	void dGoombaPie::_vf148() {
		dEn_c::_vf148();
		doStateChange(&StateID_Burst);
	}
	void dGoombaPie::_vf14C() {
		dEn_c::_vf14C();
		doStateChange(&StateID_Burst);
	}

	DoSomethingCool goombIceBlock;

	extern "C" void sub_80024C20(void);
	extern "C" void __destroy_arr(void*, void(*)(void), int, int);

	bool dGoombaPie::CreateIceActors()
	{
	    struct DoSomethingCool goombIceBlock = { 0, this->pos, {2.5, 2.5, 2.5}, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
	    this->frzMgr.Create_ICEACTORs( (void*)&goombIceBlock, 1 );
	    __destroy_arr( (void*)&goombIceBlock, sub_80024C20, 0x3C, 1 );
	    return true;
	}

	void dGoombaPie::addScoreWhenHit(void *other) {}


int dGoombaPie::onCreate() {

	// Model creation	
	allocator.link(-1, GameHeaps[0], 0, 0x20);

	this->resFile.data = getResource("pumpkin", "g3d/model.brres");
	nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("Pumpkin");
	bodyModel.setup(mdl, &allocator, 0x224, 1, 0);
	// SetupTextures_Map(&bodyModel, 0);

	mdl = this->resFile.GetResMdl("FX_Pumpkin");
	burstModel.setup(mdl, &allocator, 0x224, 1, 0);
	// SetupTextures_Map(&burstModel, 0);

	allocator.unlink();


	// Other shit
	isBursting = false;
	this->scale = (Vec){0.39, 0.39, 0.39};

	ActivePhysics::Info HitMeBaby;

	HitMeBaby.xDistToCenter = 0.0;
	HitMeBaby.yDistToCenter = 12.0;

	HitMeBaby.xDistToEdge = 8.0;
	HitMeBaby.yDistToEdge = 14.0;		

	HitMeBaby.category1 = 0x3;
	HitMeBaby.category2 = 0x0;
	HitMeBaby.bitfield1 = 0x01;
	HitMeBaby.bitfield2 = 0x820A0;
	HitMeBaby.unkShort1C = 0;
	HitMeBaby.callback = &pieCollisionCallback;

	this->aPhysics.initWithStruct(this, &HitMeBaby);
	this->aPhysics.addToList();


	// Remember to follow a goomba
	if ((settings & 0xF) == 0) {
		Goomber = (dStageActor_c*)create(EN_KURIBO, 0, &pos, &rot, 0); }
	else {
		Goomber = (dStageActor_c*)create(EN_PATA_KURIBO, 0, &pos, &rot, 0); }

	// State Changers
	doStateChange(&StateID_Follow);

	this->onExecute();
	return true;
}

int dGoombaPie::onDelete() {
	return true;
}

int dGoombaPie::onExecute() {
	acState.execute();
	this->pos = Goomber->pos;
	this->rot = Goomber->rot;
	return true;
}

int dGoombaPie::onDraw() {
	matrix.translation(pos.x, pos.y + 4.0, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	if (isBursting) {
		burstModel.setDrawMatrix(matrix);
		burstModel.setScale(&scale);
		burstModel.calcWorld(false);
		burstModel.scheduleForDrawing();
	} else {
		bodyModel.setDrawMatrix(matrix);
		bodyModel.setScale(&scale);
		bodyModel.calcWorld(false);
		bodyModel.scheduleForDrawing();
	}

	return true;
}



///////////////
// Follow State
///////////////
	void dGoombaPie::beginState_Follow() { }
	void dGoombaPie::executeState_Follow() { }
	void dGoombaPie::endState_Follow() { }

///////////////
// Burst State
///////////////
	void dGoombaPie::beginState_Burst() { 
		this->timer = 0; 
		isBursting = true;
		this->removeMyActivePhysics();
		SpawnEffect("Wm_ob_eggbreak_yw", 0, &pos, &(S16Vec){0,0,0}, &(Vec){2.0, 2.0, 2.0});
	}
	void dGoombaPie::executeState_Burst() { 
		this->Delete(1);
	}
	void dGoombaPie::endState_Burst() { }


//
// processed\../src/exceptionHandler.cpp
//

#include "game.h"
#include "fileload.h"
#include "nsmbwVer.h"

#if defined(ANOTHER_WII)
#define GAME_NAME "Another v3.2"
#elif defined(HOLIDAY_SPECIAL)
#define GAME_NAME "NerXMAS v1.12"
#elif defined(SUMMER_SUN)
#define GAME_NAME "SumSun v1.12"
#elif defined(FALLING_LEAF)
#define GAME_NAME "Newer FL v1.12"
#elif defined(NEWER_WII)
#define GAME_NAME "Newer Wii v1.30"
#else
#define GAME_NAME "The game"
#endif

const bool dsisrFun = false;
const bool gprFun = false;

u32 srr0;
const u32 dlcode = 0x80DFBCC0;

struct OSContext
{
    u32 gpr[32];
    u32 cr;
    u32 lr;
    u32 ctr;
    u32 xer;
    double fpr[32];
    u8 unknown0[8];
    u32 srr[2];
    u8 unknown1[2];
    u16 flags;
    u32 gqr[8];
    u8 unknown2[4];
    float fppr[32];
};

/*inline bool GetBit(u32 num, char m)
{
    char n = 31 - m;
    return ((num >> n) & 1);
}
void ShowMainInfo(void * _osContext, u32 dsisr, u32 dar)
{
    void* osContext = _osContext;
    //osContext += 0x198;
    osContext = (void *)(((char *)osContext) + 0x198);
    srr0 = *(u32 *)(osContext);
    //osContext += 0x4;
    osContext = (void *)(((char *)osContext) + 0x4);
    u32 srr1 = *(u32 *)(osContext);

    nw4r::db::Exception_Printf_("SRR0:    %08X\n\n", srr0);

    if (srr1Fun)
    {
        //SRR1 fun.
        //Read 16-31
        char * enabled = "enabled.";
        char * disabled = "disabled.";
        nw4r::db::Exception_Printf_("External interrupt %s\n", (GetBit(srr1, 16) ? enabled : disabled));
        nw4r::db::Exception_Printf_("Privilege level: %s\n", (GetBit(srr1, 17) ? "user" : "supervisor"));
        nw4r::db::Exception_Printf_("Floating-point %s\n", (GetBit(srr1, 18) ? enabled : disabled));
        nw4r::db::Exception_Printf_("Machine check %s\n", (GetBit(srr1, 19) ? enabled : disabled));
        //
        nw4r::db::Exception_Printf_("Single-step trace %s\n", (GetBit(srr1, 21) ? enabled : disabled));
        nw4r::db::Exception_Printf_("Branch trace %s\n", (GetBit(srr1, 22) ? enabled : disabled));
        //
        //
        nw4r::db::Exception_Printf_("Exception prefix: %s\n", (GetBit(srr1, 25) ? "0xFFF....." : "0x000....."));
        nw4r::db::Exception_Printf_("Instruction address translation %s\n", (GetBit(srr1, 26) ? enabled : disabled));
        nw4r::db::Exception_Printf_("Data address translation %s\n", (GetBit(srr1, 27) ? enabled : disabled));
        //
        nw4r::db::Exception_Printf_("Process is %s (performance)\n", (GetBit(srr1, 29) ? "marked" : "not marked"));
        nw4r::db::Exception_Printf_("Exception is %s\n", (GetBit(srr1, 30) ? "recoverable" : "not recoverable"));
        nw4r::db::Exception_Printf_("%s endian mode\n\n", (GetBit(srr1, 31) ? "Little" : "Big"));
    }

    if (dsisrFun)
    {
        //DSISR fun.
        if (GetBit(dsisr, 0)) nw4r::db::Exception_Printf_("Crashed by load/store instruction\n");
        if (GetBit(dsisr, 1)) nw4r::db::Exception_Printf_("Attempted access translation not found\n");
        if (GetBit(dsisr, 4)) nw4r::db::Exception_Printf_("A memory access in not permitted\n");
        if (GetBit(dsisr, 5)) nw4r::db::Exception_Printf_("Store/load instruction performed at a bad address\n");
        nw4r::db::Exception_Printf_("Crashed by %s instruction\n", (GetBit(dsisr, 6) ? "store" : "load"));
        if (GetBit(dsisr, 9)) nw4r::db::Exception_Printf_("DABR match occurs\n");
        if (GetBit(dsisr, 11)) nw4r::db::Exception_Printf_("Instruction is eciwx or ecowx\n");
        nw4r::db::Exception_Printf_("\n");
    }
}*/
char *GetRegionAndVersion()
{
	NSMBWVer version = getNsmbwVer();
	switch(version)
	{
		case pal:
			return "PALv1";
		case pal2:
			return "PALv2";
		case ntsc:
			return "NTSCv1";
		case ntsc2:
			return "NTSCv2";
		case jpn:
			return "JPNv1";
		case jpn2:
			return "JPNv2";
		case kor:
			return "kor";
		case twn:
			return "twn";
		default:
			return "UNKNOWN";
	}
}
char *GetErrorDescription(u16 OSError)
{
    char *desc[] =
        {
            "SYSTEM RESET",
            "MACHINE CHECK",
            "DSI", "ISI",
            "EXTERNAL INTERRUPT",
            "ALIGNMENT",
            "PROGRAM",
            "FLOATING POINT",
            "DECREMENTER",
            "SYSTEM CALL",
            "TRACE",
            "PERFORMANCE MONITOR",
            "BREAKPOINT",
            "SYSTEM INTERRUPT",
            "THERMAL INTERRUPT",
            "PROTECTION",
            "FPE"};
    return desc[OSError];
}
void PrintContext(u16 OSError, void *_osContext, u32 _dsisr, u32 _dar)
{
    OSContext *osContext = (OSContext *)_osContext;
	
    nw4r::db::Exception_Printf_("Whoops! " GAME_NAME " [%s] has crashed - %s\n\nPlease send the information below to\nnewer@newerteam.com\nYou can scroll through this report using the D-Pad.\n\n", GetRegionAndVersion(), GetErrorDescription(OSError));
    nw4r::db::Exception_Printf_("SRR0: %08X | DSISR: %08X | DAR: %08X\n", osContext->srr[0]);

    if (gprFun)
    {
        int i = 0;
        do
        {
            nw4r::db::Exception_Printf_("R%02d:%08X  R%02d:%08X  R%02d:%08X\n", i, osContext->gpr[i], i + 0xB, osContext->gpr[i + 0xB], i + 0x16, osContext->gpr[i + 0x16]);
            i++;
        } while (i < 10);
        nw4r::db::Exception_Printf_("R%02d:%08X  R%02d:%08X\n\n", 10, osContext->gpr[10], 0x15, osContext->gpr[0x15]);
    }

    //Stack trace
    //nw4r::db::Exception_Printf_("\nException info trace (most recent on top):\n");
    int i = 0;
    u32 *stackPointer = (u32 *)((char *)nw4r::db::sException + 0x33C);
    do
    {
        if (stackPointer == 0 || (u32)stackPointer == 0xFFFFFFFF)
            break;

        nw4r::db::Exception_Printf_("%08X", stackPointer[1]);
        if (stackPointer[1] >= dlcode)
        {
            nw4r::db::Exception_Printf_(" - %08X NewerASM", stackPointer[1] - dlcode);
        }
        nw4r::db::Exception_Printf_("\n");

        i++;
        stackPointer = (u32 *)*stackPointer;
    } while (i < 0x10);
    //while (true);
}
//
// processed\../src/actors.cpp
//

#include <actors.h>
#include "nsmbwVer.h"

// Get the appropriate ID based on game version
Actors translateActorID(Actors id)
{
	NSMBWVer version = getNsmbwVer();
	switch(version)
	{
		case pal:
		case pal2:
		case ntsc:
		case ntsc2:
		case jpn:
		case jpn2:
		default: // unknown
			return adjustID(id);
			break;
		case kor:
		case twn:
			return id;
			break;
	}
}

Actors adjustID(Actors id)
{
	if(id > 703)
		return Actors(id - 2);
	if(id == 702 || id == 703) // wtf
		return Actors(id + 2);
	
	return id; // Actor is below id 702
}

//
// processed\../src/nsmbwVer.cpp
//

#include "nsmbwVer.h"

NSMBWVer getNsmbwVer()
{	
	u32 checkVer = *((u32*)0x800CF6CC);
	u32 checkKrTw;
	
	switch(checkVer)
	{
		case 0x40820030:
			return pal;
			break;
		case 0x40820038:
			return pal2;
			break;
		case 0x48000465:
			return ntsc;
			break;
		case 0x2c030000:
			return ntsc2;
			break;
		case 0x480000b4:
			return jpn;
			break;
		case 0x4082000c:
			return jpn2;
			break;
		case 0x38a00001:
			checkKrTw = *((u32*)0x80004238);
			if(checkKrTw == 0x6021c8e0) return kor;
			if(checkKrTw == 0x6021ace0) return twn;
			break;
	}
	return pal; // To appease the compiler warning gods
}
