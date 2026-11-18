# Empire.Game.Engine — полная структура проекта

```text
Empire.Game.Engine/
├── xmake.lua
├── Empire.Scripts/
│   ├── Linux/      (*.sh)
│   ├── MacOS/      (*.command)
│   └── Windows/    (*.bat)
├── README.md
├── .gitignore
│
├── Empire.Engine/
│   ├── Engine.Kernel/
│   │   ├── Kernel.Algorithms/
│   │   ├── Kernel.Assert/
│   │   ├── Kernel.Compiler/
│   │   ├── Kernel.Config/
│   │   ├── Kernel.Containers/
│   │   ├── Kernel.Environment/
│   │   ├── Kernel.Events/
│   │   ├── Kernel.FileSystem/
│   │   ├── Kernel.Jobs/
│   │   ├── Kernel.Library/
│   │   ├── Kernel.Logging/
│   │   ├── Kernel.Memory/
│   │   ├── Kernel.Network/
│   │   ├── Kernel.Process/
│   │   ├── Kernel.Profiler/
│   │   ├── Kernel.Reflection/
│   │   ├── Kernel.Services/
│   │   ├── Kernel.Streams/
│   │   ├── Kernel.Sync/
│   │   ├── Kernel.System/
│   │   ├── Kernel.Threading/
│   │   ├── Kernel.Time/
│   │   └── Kernel.Types/
│   │
│   ├── Engine.Math/
│   │   ├── Math.Color/
│   │   ├── Math.Geometry/
│   │   │   ├── Geometry.2D/
│   │   │   └── Geometry.3D/
│   │   ├── Math.Matrix/
│   │   ├── Math.Quaternion/
│   │   ├── Math.Random/
│   │   ├── Math.Scalar/
│   │   ├── Math.Simd/
│   │   ├── Math.Transform/
│   │   │   ├── Transform.2D/
│   │   │   ├── Transform.3D/
│   │   │   └── Transform.World/
│   │   └── Math.Vector/
│   │
│   ├── Engine.Platform/
│   │   ├── Platform.App/
│   │   ├── Platform.Audio/
│   │   ├── Platform.Clipboard/
│   │   ├── Platform.Cursor/
│   │   ├── Platform.Dialog/
│   │   ├── Platform.Events/
│   │   ├── Platform.Input/
│   │   ├── Platform.Monitor/
│   │   ├── Platform.Surface/
│   │   ├── Platform.ThirdParty/
│   │   └── Platform.Window/
│   │
│   ├── Engine.Graphics/
│   │   ├── Graphics.Adapter/
│   │   ├── Graphics.BindGroup/
│   │   ├── Graphics.Buffer/
│   │   ├── Graphics.CommandBuffer/
│   │   ├── Graphics.Common/
│   │   ├── Graphics.Device/
│   │   ├── Graphics.Fence/
│   │   ├── Graphics.Instance/
│   │   ├── Graphics.Pipeline/
│   │   ├── Graphics.Query/
│   │   ├── Graphics.Queue/
│   │   ├── Graphics.RenderPass/
│   │   ├── Graphics.Sampler/
│   │   ├── Graphics.Semaphore/
│   │   ├── Graphics.Shader/
│   │   ├── Graphics.SwapChain/
│   │   ├── Graphics.Texture/
│   │   └── Graphics.ThirdParty/
│   │
│   ├── Engine.Rendering/
│   │   ├── Rendering.Culling/
│   │   ├── Rendering.Debug/
│   │   ├── Rendering.Lighting/
│   │   ├── Rendering.Material/
│   │   ├── Rendering.Mesh/
│   │   ├── Rendering.PostProcess/
│   │   ├── Rendering.RenderGraph/
│   │   ├── Rendering.Scene/
│   │   ├── Rendering.Shader/
│   │   ├── Rendering.Shadow/
│   │   └── Rendering.View/
│   │
│   ├── Engine.Physics/
│   │   ├── Physics.Body/
│   │   ├── Physics.Character/
│   │   ├── Physics.Collision/
│   │   ├── Physics.Constraint/
│   │   ├── Physics.Debug/
│   │   ├── Physics.Query/
│   │   ├── Physics.Serialization/
│   │   ├── Physics.Shape/
│   │   ├── Physics.SoftBody/
│   │   ├── Physics.Solver/
│   │   └── Physics.Vehicle/
│   │
│   ├── Engine.Animation/
│   │   ├── Animation.Blend/
│   │   ├── Animation.Clip/
│   │   ├── Animation.IK/
│   │   ├── Animation.Pose/
│   │   ├── Animation.Retarget/
│   │   ├── Animation.Skeleton/
│   │   └── Animation.StateMachine/
│   │
│   ├── Engine.Sound/
│   │   ├── Sound.Decoder/
│   │   ├── Sound.Effects/
│   │   ├── Sound.Listener/
│   │   ├── Sound.Mixer/
│   │   ├── Sound.Source/
│   │   ├── Sound.Spatial/
│   │   └── Sound.Streaming/
│   │
│   ├── Engine.AI/
│   │   ├── AI.BehaviorTree/
│   │   ├── AI.Blackboard/
│   │   ├── AI.Navigation/
│   │   ├── AI.Perception/
│   │   └── AI.Steering/
│   │
│   ├── Engine.Networking/
│   │   ├── Networking.Prediction/
│   │   ├── Networking.Protocol/
│   │   ├── Networking.Replication/
│   │   ├── Networking.Session/
│   │   └── Networking.Transport/
│   │
│   ├── Engine.ECS/
│   │   ├── ECS.Archetype/
│   │   ├── ECS.Change/
│   │   ├── ECS.Command/
│   │   ├── ECS.Component/
│   │   ├── ECS.Entity/
│   │   ├── ECS.Query/
│   │   ├── ECS.Resource/
│   │   ├── ECS.Schedule/
│   │   ├── ECS.Snapshot/
│   │   ├── ECS.System/
│   │   └── ECS.World/
│   │
│   ├── Engine.Nodes/
│   │   ├── Nodes.Kernel/
│   │   │   ├── Kernel.Node/
│   │   │   ├── Kernel.Node2D/
│   │   │   ├── Kernel.Node3D/
│   │   │   ├── Kernel.Canvas/
│   │   │   ├── Kernel.CanvasLvl/
│   │   │   ├── Kernel.CanvasGrp/
│   │   │   ├── Kernel.Viewport/
│   │   │   ├── Kernel.SubView/
│   │   │   ├── Kernel.SubViewBox/
│   │   │   ├── Kernel.Window/
│   │   │   ├── Kernel.Timer/
│   │   │   ├── Kernel.HTTPReq/
│   │   │   ├── Kernel.ResPreload/
│   │   │   ├── Kernel.InstStub/
│   │   │   ├── Kernel.Missing/
│   │   │   ├── Kernel.NetSpawn/
│   │   │   ├── Kernel.NetSync/
│   │   │   └── Kernel.StatusInd/
│   │   │
│   │   ├── Nodes.Hierarchy/
│   │   ├── Nodes.Prefab/
│   │   ├── Nodes.Scene/
│   │   ├── Nodes.Serialization/
│   │   ├── Nodes.Types/
│   │   ├── Nodes.Camera/
│   │   │
│   │   ├── Nodes.2D/
│   │   │   ├── 2D.AnimSprite2D/
│   │   │   ├── 2D.BackBufCopy/
│   │   │   ├── 2D.Bone2D/
│   │   │   ├── 2D.Camera2D/
│   │   │   ├── 2D.CanvasModulate/
│   │   │   ├── 2D.CollPolygon2D/
│   │   │   ├── 2D.CollShape2D/
│   │   │   ├── 2D.Line2D/
│   │   │   ├── 2D.LightOcc2D/
│   │   │   ├── 2D.Mesh2D/
│   │   │   ├── 2D.MultiMesh2D/
│   │   │   ├── 2D.Para2D/
│   │   │   ├── 2D.ParaBG/
│   │   │   ├── 2D.ParaLayer/
│   │   │   ├── 2D.Path2D/
│   │   │   ├── 2D.PathFollow2D/
│   │   │   ├── 2D.PhysBone2D/
│   │   │   ├── 2D.Polygon2D/
│   │   │   ├── 2D.RemoteXform2D/
│   │   │   ├── 2D.Skeleton2D/
│   │   │   ├── 2D.Sprite2D/
│   │   │   ├── 2D.TileMap/
│   │   │   ├── 2D.TileMapLayer/
│   │   │   ├── 2D.TouchButton/
│   │   │   ├── 2D.ScreenEnable2D/
│   │   │   └── 2D.ScreenNote2D/
│   │   │
│   │   ├── Nodes.3D/
│   │   │   ├── 3D.AnimSprite3D/
│   │   │   ├── 3D.BoneAttach3D/
│   │   │   ├── 3D.Camera3D/
│   │   │   ├── 3D.CSGBox3D/
│   │   │   ├── 3D.CSGCombine3D/
│   │   │   ├── 3D.CSGCylinder3D/
│   │   │   ├── 3D.CSGMesh3D/
│   │   │   ├── 3D.CSGPolygon3D/
│   │   │   ├── 3D.CSGSphere3D/
│   │   │   ├── 3D.CSGTorus3D/
│   │   │   ├── 3D.Decal/
│   │   │   ├── 3D.FogVolume/
│   │   │   ├── 3D.Geometry3D/
│   │   │   ├── 3D.GridMap/
│   │   │   ├── 3D.MeshImport3D/
│   │   │   ├── 3D.LightmapGI/
│   │   │   ├── 3D.LightProbe/
│   │   │   ├── 3D.Marker3D/
│   │   │   ├── 3D.Mesh3D/
│   │   │   ├── 3D.MultiMesh3D/
│   │   │   ├── 3D.Occluder3D/
│   │   │   ├── 3D.Path3D/
│   │   │   ├── 3D.PathFollow3D/
│   │   │   ├── 3D.PhysBone3D/
│   │   │   ├── 3D.PhysBoneSim3D/
│   │   │   ├── 3D.ReflectionProbe/
│   │   │   ├── 3D.RemoteXform3D/
│   │   │   ├── 3D.RootMotion/
│   │   │   ├── 3D.Skeleton3D/
│   │   │   ├── 3D.Sprite3D/
│   │   │   ├── 3D.VoxelGI/
│   │   │   ├── 3D.ScreenEnable3D/
│   │   │   ├── 3D.ScreenNote3D/
│   │   │   ├── 3D.Visual3D/
│   │   │   └── 3D.WorldEnv/
│   │   │
│   │   ├── Nodes.UI/
│   │   │   ├── UI.AcceptDialog/
│   │   │   ├── UI.AspectCont/
│   │   │   ├── UI.BaseButton/
│   │   │   ├── UI.BoxCont/
│   │   │   ├── UI.Button/
│   │   │   ├── UI.CenterCont/
│   │   │   ├── UI.CheckBox/
│   │   │   ├── UI.CheckButton/
│   │   │   ├── UI.CodeEdit/
│   │   │   ├── UI.ColorPicker/
│   │   │   ├── UI.ColorButton/
│   │   │   ├── UI.ColorRect/
│   │   │   ├── UI.ConfirmDlg/
│   │   │   ├── UI.Container/
│   │   │   ├── UI.Control/
│   │   │   ├── UI.FileDialog/
│   │   │   ├── UI.FlowCont/
│   │   │   ├── UI.FoldCont/
│   │   │   ├── UI.GraphEdit/
│   │   │   ├── UI.GraphElement/
│   │   │   ├── UI.GraphFrame/
│   │   │   ├── UI.GraphNode/
│   │   │   ├── UI.GridCont/
│   │   │   ├── UI.HBox/
│   │   │   ├── UI.HFlow/
│   │   │   ├── UI.HScrollBar/
│   │   │   ├── UI.HSeparator/
│   │   │   ├── UI.HSlider/
│   │   │   ├── UI.HSplit/
│   │   │   ├── UI.ItemList/
│   │   │   ├── UI.Label/
│   │   │   ├── UI.LineEdit/
│   │   │   ├── UI.LinkButton/
│   │   │   ├── UI.MarginCont/
│   │   │   ├── UI.MenuBar/
│   │   │   ├── UI.MenuButton/
│   │   │   ├── UI.NinePatch/
│   │   │   ├── UI.OptionButton/
│   │   │   ├── UI.PanelCont/
│   │   │   ├── UI.Popup/
│   │   │   ├── UI.PopupMenu/
│   │   │   ├── UI.PopupPanel/
│   │   │   ├── UI.ProgressBar/
│   │   │   ├── UI.RefRect/
│   │   │   ├── UI.RichLabel/
│   │   │   ├── UI.ScrollBar/
│   │   │   ├── UI.ScrollCont/
│   │   │   ├── UI.Separator/
│   │   │   ├── UI.Slider/
│   │   │   ├── UI.SpinBox/
│   │   │   ├── UI.SplitCont/
│   │   │   ├── UI.TabBar/
│   │   │   ├── UI.TabCont/
│   │   │   ├── UI.TextEdit/
│   │   │   ├── UI.TextureButton/
│   │   │   ├── UI.TextureProgress/
│   │   │   ├── UI.TextureRect/
│   │   │   ├── UI.Tree/
│   │   │   ├── UI.VBox/
│   │   │   ├── UI.VFlow/
│   │   │   ├── UI.VScrollBar/
│   │   │   ├── UI.VSeparator/
│   │   │   ├── UI.VSlider/
│   │   │   └── UI.VSplit/
│   │   │
│   │   ├── Nodes.Anim/
│   │   │   ├── Anim.AimMod3D/
│   │   │   ├── Anim.Mixer/
│   │   │   ├── Anim.Player/
│   │   │   ├── Anim.Tree/
│   │   │   ├── Anim.BoneConst3D/
│   │   │   ├── Anim.BoneTwist3D/
│   │   │   ├── Anim.CCDIK3D/
│   │   │   ├── Anim.FABRIK3D/
│   │   │   ├── Anim.IKMod3D/
│   │   │   ├── Anim.LookAt3D/
│   │   │   ├── Anim.SkelIK3D/
│   │   │   ├── Anim.SkelMod3D/
│   │   │   ├── Anim.SpringBone3D/
│   │   │   └── Anim.TwoIK3D/
│   │   │
│   │   ├── Nodes.Phys/
│   │   │   ├── Phys.AnimBody2D/
│   │   │   ├── Phys.AnimBody3D/
│   │   │   ├── Phys.Area2D/
│   │   │   ├── Phys.Area3D/
│   │   │   ├── Phys.CharBody2D/
│   │   │   ├── Phys.CharBody3D/
│   │   │   ├── Phys.Coll2D/
│   │   │   ├── Phys.Coll3D/
│   │   │   ├── Phys.SpringJoint2D/
│   │   │   ├── Phys.GrooveJoint2D/
│   │   │   ├── Phys.Joint6DOF3D/
│   │   │   ├── Phys.HingeJoint3D/
│   │   │   ├── Phys.Joint2D/
│   │   │   ├── Phys.Joint3D/
│   │   │   ├── Phys.PinJoint2D/
│   │   │   ├── Phys.PinJoint3D/
│   │   │   ├── Phys.RayCast2D/
│   │   │   ├── Phys.RayCast3D/
│   │   │   ├── Phys.RigidBody2D/
│   │   │   ├── Phys.RigidBody3D/
│   │   │   ├── Phys.ShapeCast2D/
│   │   │   ├── Phys.ShapeCast3D/
│   │   │   ├── Phys.SliderJoint3D/
│   │   │   ├── Phys.SoftBody3D/
│   │   │   ├── Phys.SpringArm3D/
│   │   │   ├── Phys.StaticBody2D/
│   │   │   ├── Phys.StaticBody3D/
│   │   │   ├── Phys.VehicleBody3D/
│   │   │   └── Phys.VehicleWheel3D/
│   │   │
│   │   ├── Nodes.Nav/
│   │   │   ├── Nav.Agent2D/
│   │   │   ├── Nav.Agent3D/
│   │   │   ├── Nav.Link2D/
│   │   │   ├── Nav.Link3D/
│   │   │   ├── Nav.Obstacle2D/
│   │   │   ├── Nav.Obstacle3D/
│   │   │   ├── Nav.Region2D/
│   │   │   └── Nav.Region3D/
│   │   │
│   │   ├── Nodes.Render/
│   │   │   ├── Render.DirLight2D/
│   │   │   ├── Render.DirLight3D/
│   │   │   ├── Render.Light2D/
│   │   │   ├── Render.Light3D/
│   │   │   ├── Render.OmniLight3D/
│   │   │   ├── Render.PointLight2D/
│   │   │   ├── Render.ShaderVars/
│   │   │   └── Render.SpotLight3D/
│   │   │
│   │   ├── Nodes.Particle/
│   │   │   ├── Particle.CPUParts2D/
│   │   │   ├── Particle.CPUParts3D/
│   │   │   ├── Particle.GPUParts2D/
│   │   │   ├── Particle.GPUParts3D/
│   │   │   ├── Particle.Attractor3D/
│   │   │   ├── Particle.Collision3D/
│   │   │   ├── Particle.Box3D/
│   │   │   ├── Particle.Height3D/
│   │   │   ├── Particle.SDF3D/
│   │   │   └── Particle.Sphere3D/
│   │   │
│   │   ├── Nodes.Audio/
│   │   │   ├── Audio.Listener2D/
│   │   │   ├── Audio.Listener3D/
│   │   │   ├── Audio.Player/
│   │   │   ├── Audio.Player2D/
│   │   │   └── Audio.Player3D/
│   │   │
│   │   ├── Nodes.Video/
│   │   │   └── Video.Player/
│   │   │
│   │   ├── Nodes.XR/
│   │   │   ├── XR.Anchor3D/
│   │   │   ├── XR.Camera3D/
│   │   │   ├── XR.Controller3D/
│   │   │   ├── XR.Hand/
│   │   │   ├── XR.Node3D/
│   │   │   ├── XR.Origin3D/
│   │   │   ├── XR.Layer/
│   │   │   ├── XR.LayerCylinder/
│   │   │   ├── XR.LayerEq/
│   │   │   ├── XR.LayerQuad/
│   │   │   ├── XR.Model/
│   │   │   ├── XR.ModelMgr/
│   │   │   └── XR.VisMask/
│   │   │
│   │   └── Nodes.Editor/
│   │       ├── Editor.CmdPalette/
│   │       ├── Editor.Dock/
│   │       ├── Editor.FileDialog/
│   │       ├── Editor.FileSystem/
│   │       ├── Editor.Inspector/
│   │       ├── Editor.Plugin/
│   │       ├── Editor.Property/
│   │       ├── Editor.ResPicker/
│   │       ├── Editor.ResPreview/
│   │       ├── Editor.ScriptPicker/
│   │       ├── Editor.SpinSlider/
│   │       ├── Editor.Toaster/
│   │       ├── Editor.FSDock/
│   │       ├── Editor.ScriptNewDlg/
│   │       ├── Editor.ScriptEditor/
│   │       └── Editor.ScriptEditBase/
│   │
│   ├── Engine.Assets/
│   │   ├── Assets.Cache/
│   │   ├── Assets.Dependency/
│   │   ├── Assets.Handle/
│   │   ├── Assets.Importer/
│   │   ├── Assets.Loader/
│   │   ├── Assets.Package/
│   │   ├── Assets.Registry/
│   │   └── Assets.Streaming/
│   │
│   ├── Engine.Input/
│   │   ├── Input.Action/
│   │   ├── Input.Binding/
│   │   └── Input.Context/
│   │
│   ├── Engine.Gui/
│   │   ├── Gui.Draw/
│   │   ├── Gui.Layout/
│   │   ├── Gui.Style/
│   │   ├── Gui.Text/
│   │   ├── Gui.Theme/
│   │   └── Gui.Widget/
│   │
│   └── Engine.Abilities/
│       ├── Abilities.Ability/
│       ├── Abilities.Attribute/
│       ├── Abilities.Cooldown/
│       ├── Abilities.Cue/
│       ├── Abilities.Effect/
│       └── Abilities.Tag/
│
├── Empire.Runtime/
│   ├── Runtime.Editor/
│   │   ├── Editor.Framework/
│   │   ├── Editor.Gizmo/
│   │   ├── Editor.Panel/
│   │   ├── Editor.Plugin/
│   │   ├── Editor.Tool/
│   │   └── Editor.Viewport/
│   │
│   ├── Runtime.Game/
│   │   ├── Game.Shared/
│   │   ├── Game.Launcher/
│   │   ├── Game.Debug/
│   │   ├── Game.DebugGame/
│   │   ├── Game.Development/
│   │   ├── Game.Test/
│   │   └── Game.Shipping/
│   │
│   ├── Runtime.Client/
│   │   ├── Client.Shared/
│   │   ├── Client.Launcher/
│   │   ├── Client.Debug/
│   │   ├── Client.DebugGame/
│   │   ├── Client.Development/
│   │   ├── Client.Test/
│   │   └── Client.Shipping/
│   │
│   ├── Runtime.Server/
│   │   ├── Server.Shared/
│   │   ├── Server.Launcher/
│   │   ├── Server.Debug/
│   │   ├── Server.DebugGame/
│   │   ├── Server.Development/
│   │   ├── Server.Test/
│   │   └── Server.Shipping/
│   │
│   ├── Runtime.Scripting/
│   │   ├── Scripting.Cpp/
│   │   ├── Scripting.CSharp/
│   │   ├── Scripting.Lua/
│   │   └── Scripting.Wasm/
│   │
│   ├── Runtime.Shared/
│   │   ├── Shared.Application/
│   │   ├── Shared.Diagnostics/
│   │   ├── Shared.GameLoop/
│   │   └── Shared.Modules/
│   │
│   ├── Runtime.Tests/
│   │   ├── Tests.Kernel/
│   │   ├── Tests.Math/
│   │   ├── Tests.Mock/
│   │   ├── Tests.Platform/
│   │   ├── Tests.Graphics/
│   │   ├── Tests.Rendering/
│   │   ├── Tests.Physics/
│   │   ├── Tests.Animation/
│   │   ├── Tests.Sound/
│   │   ├── Tests.AI/
│   │   ├── Tests.Networking/
│   │   ├── Tests.ECS/
│   │   ├── Tests.Nodes/
│   │   ├── Tests.Assets/
│   │   ├── Tests.Input/
│   │   ├── Tests.Gui/
│   │   └── Tests.Abilities/
│   │
│   └── Runtime.Tools/
│       ├── Tools.AssetCompiler/
│       ├── Tools.Build/
│       ├── Tools.Cli/
│       ├── Tools.Packager/
│       ├── Tools.Profiler/
│       └── Tools.ShaderCompiler/
│
├── Empire.Content/
│   ├── Content.Engine/
│   │   ├── Engine.Fonts/
│   │   ├── Engine.Icons/
│   │   └── Engine.Shaders/
│   └── Content.Editor/
│
├── Empire.Samples/
│   ├── Samples.Kernel/
│   ├── Samples.Math/
│   ├── Samples.Platform/
│   ├── Samples.Graphics/
│   ├── Samples.Rendering/
│   ├── Samples.Physics/
│   ├── Samples.Animation/
│   ├── Samples.Sound/
│   ├── Samples.AI/
│   ├── Samples.Networking/
│   ├── Samples.ECS/
│   ├── Samples.Nodes/
│   ├── Samples.Assets/
│   ├── Samples.Input/
│   ├── Samples.Gui/
│   └── Samples.Abilities/
│
├── Empire.Docs/
│   ├── Docs.Architecture/
│   ├── Docs.Guides/
│   ├── Docs.Reference/
│   └── Docs.ThirdParty/
│
└── Empire.Build/
    └── Build.<Платформа>.<Конфигурация>/
        ├── Build.Engine/
        ├── Build.Runtime/
        ├── Build.Content/
        ├── Build.Docs/
        └── .Obj/
```

## Примечания

- `Empire.Build/` — генерируемая директория, добавленная в `.gitignore`.
- `Build.<Платформа>.<Конфигурация>/` — например, `Build.Linux.Debug/`, `Build.Windows.Development/`, `Build.MacOS.Shipping/`.
- `Runtime.Game`, `Runtime.Client` и `Runtime.Server` показаны как отдельные runtime-проекты. Внутри них перечислены конфигурации сборки; конкретные сочетания конфигураций и целей можно уточнять в build-системе.
- `Runtime.Scripting` содержит интеграции C++, C#, Lua и WebAssembly.
- `Kernel.Jobs` предназначен для пула потоков, барьеров, графов задач и `parallel_for`.
- `Kernel.Network` содержит низкоуровневую сетевую основу (сокеты); протоколы и репликация находятся в `Engine.Networking`.
- `Engine.Gui` формирует draw-list и не зависит от `Engine.Rendering`.
- `Engine.Nodes` — отдельная объектная модель сцены; `Nodes.UI` интегрируется с `Engine.Gui`, а физические, графические и аудиовозможности узлов используют соответствующие технологии движка.
- `Nodes.*` — названия модулей; класс внутри одноимённого модуля может иметь имя без префикса, например `Nodes.Camera3D/Camera3D`.
- Список `Nodes.*` составлен по мотивам Godot 4 и является предварительной инвентаризацией классов, а не утверждением о полном совпадении с конкретным Godot ClassDB.
- `ECS.Change/` и `ECS.Snapshot/` оставлены как необязательные направления.
- `Nodes` с пометкой о поддержке опций можно включать/исключать при конфигурации сборки.
