#!/usr/bin/env ruby
#
# FreeAutotune Xcode Project Generator
# Generates a basic Xcode project structure for AUV3 plugin
#
# Usage: ruby generate_xcode_project.rb
#

require 'fileutils'
require 'securerandom'

class XcodeProjectGenerator
  def initialize
    @project_name = "FreeAutotune"
    @bundle_id = "com.freeaudio.FreeAutotune"
    @au_bundle_id = "#{@bundle_id}.AudioUnit"
    @project_dir = "#{@project_name}.xcodeproj"
  end

  def generate
    puts "🎵 FreeAutotune Xcode Project Generator"
    puts "======================================\n\n"

    create_project_structure
    create_pbxproj_file
    create_xcscheme_files

    puts "\n✅ Xcode project generated successfully!"
    puts "\nNext steps:"
    puts "1. Open #{@project_name}.xcodeproj in Xcode"
    puts "2. Add source files to appropriate targets"
    puts "3. Configure signing & capabilities"
    puts "4. Build and test\n\n"
  end

  private

  def create_project_structure
    puts "📁 Creating project structure..."

    FileUtils.mkdir_p(@project_dir)
    FileUtils.mkdir_p("#{@project_dir}/xcshareddata/xcschemes")
    FileUtils.mkdir_p("#{@project_dir}/project.xcworkspace")
    FileUtils.mkdir_p("#{@project_dir}/project.xcworkspace/xcshareddata")
  end

  def create_pbxproj_file
    puts "📝 Generating project.pbxproj..."

    # Generate UUIDs for Xcode objects
    uuids = {}
    [
      :project, :app_target, :au_target,
      :app_group, :au_group, :source_group, :dsp_group,
      :app_build_config_debug, :app_build_config_release,
      :au_build_config_debug, :au_build_config_release,
      :project_build_config_debug, :project_build_config_release,
      :app_product_ref, :au_product_ref
    ].each do |key|
      uuids[key] = SecureRandom.hex(12).upcase
    end

    pbxproj_content = <<~PBXPROJ
      // !$*UTF8*$!
      {
      	archiveVersion = 1;
      	classes = {
      	};
      	objectVersion = 54;
      	objects = {

      /* Begin PBXFileReference section */
      		#{uuids[:app_product_ref]} /* #{@project_name}.app */ = {isa = PBXFileReference; explicitFileType = wrapper.application; includeInIndex = 0; path = "#{@project_name}.app"; sourceTree = BUILT_PRODUCTS_DIR; };
      		#{uuids[:au_product_ref]} /* #{@project_name}AU.appex */ = {isa = PBXFileReference; explicitFileType = "wrapper.app-extension"; includeInIndex = 0; path = "#{@project_name}AU.appex"; sourceTree = BUILT_PRODUCTS_DIR; };
      /* End PBXFileReference section */

      /* Begin PBXGroup section */
      		#{uuids[:project]} = {
      			isa = PBXGroup;
      			children = (
      				#{uuids[:app_group]} /* #{@project_name} */,
      				#{uuids[:au_group]} /* #{@project_name}AU */,
      				#{uuids[:source_group]} /* Source */,
      			);
      			sourceTree = "<group>";
      		};
      		#{uuids[:app_group]} /* #{@project_name} */ = {
      			isa = PBXGroup;
      			children = (
      			);
      			path = HostApp;
      			sourceTree = "<group>";
      		};
      		#{uuids[:au_group]} /* #{@project_name}AU */ = {
      			isa = PBXGroup;
      			children = (
      			);
      			path = AudioUnit;
      			sourceTree = "<group>";
      		};
      		#{uuids[:source_group]} /* Source */ = {
      			isa = PBXGroup;
      			children = (
      				#{uuids[:dsp_group]} /* DSP */,
      			);
      			path = Source;
      			sourceTree = "<group>";
      		};
      		#{uuids[:dsp_group]} /* DSP */ = {
      			isa = PBXGroup;
      			children = (
      			);
      			path = DSP;
      			sourceTree = "<group>";
      		};
      /* End PBXGroup section */

      /* Begin PBXNativeTarget section */
      		#{uuids[:app_target]} /* #{@project_name} */ = {
      			isa = PBXNativeTarget;
      			buildConfigurationList = APPCONFIG_LIST;
      			buildPhases = (
      			);
      			buildRules = (
      			);
      			dependencies = (
      			);
      			name = "#{@project_name}";
      			productName = "#{@project_name}";
      			productReference = #{uuids[:app_product_ref]};
      			productType = "com.apple.product-type.application";
      		};
      		#{uuids[:au_target]} /* #{@project_name}AU */ = {
      			isa = PBXNativeTarget;
      			buildConfigurationList = AUCONFIG_LIST;
      			buildPhases = (
      			);
      			buildRules = (
      			);
      			dependencies = (
      			);
      			name = "#{@project_name}AU";
      			productName = "#{@project_name}AU";
      			productReference = #{uuids[:au_product_ref]};
      			productType = "com.apple.product-type.app-extension";
      		};
      /* End PBXNativeTarget section */

      /* Begin PBXProject section */
      		#{uuids[:project]} /* Project object */ = {
      			isa = PBXProject;
      			attributes = {
      				BuildIndependentTargetsInParallel = 1;
      				LastSwiftUpdateCheck = 1400;
      				LastUpgradeCheck = 1400;
      				TargetAttributes = {
      					#{uuids[:app_target]} = {
      						CreatedOnToolsVersion = 14.0;
      					};
      					#{uuids[:au_target]} = {
      						CreatedOnToolsVersion = 14.0;
      					};
      				};
      			};
      			buildConfigurationList = #{uuids[:project_build_config_debug]};
      			compatibilityVersion = "Xcode 13.0";
      			developmentRegion = en;
      			hasScannedForEncodings = 0;
      			knownRegions = (
      				en,
      				Base,
      			);
      			mainGroup = #{uuids[:project]};
      			productRefGroup = #{uuids[:project]};
      			projectDirPath = "";
      			projectRoot = "";
      			targets = (
      				#{uuids[:app_target]},
      				#{uuids[:au_target]},
      			);
      		};
      /* End PBXProject section */

      	};
      	rootObject = #{uuids[:project]} /* Project object */;
      }
    PBXPROJ

    File.write("#{@project_dir}/project.pbxproj", pbxproj_content)
  end

  def create_xcscheme_files
    puts "🎨 Creating Xcode schemes..."

    # App scheme
    app_scheme = <<~SCHEME
      <?xml version="1.0" encoding="UTF-8"?>
      <Scheme
         LastUpgradeVersion = "1400"
         version = "1.3">
         <BuildAction
            parallelizeBuildables = "YES"
            buildImplicitDependencies = "YES">
            <BuildActionEntries>
               <BuildActionEntry
                  buildForTesting = "YES"
                  buildForRunning = "YES"
                  buildForProfiling = "YES"
                  buildForArchiving = "YES"
                  buildForAnalyzing = "YES">
                  <BuildableReference
                     BuildableIdentifier = "primary"
                     BlueprintIdentifier = "APP_TARGET_ID"
                     BuildableName = "#{@project_name}.app"
                     BlueprintName = "#{@project_name}"
                     ReferencedContainer = "container:#{@project_name}.xcodeproj">
                  </BuildableReference>
               </BuildActionEntry>
            </BuildActionEntries>
         </BuildAction>
         <LaunchAction
            buildConfiguration = "Debug"
            selectedDebuggerIdentifier = "Xcode.DebuggerFoundation.Debugger.LLDB"
            selectedLauncherIdentifier = "Xcode.DebuggerFoundation.Launcher.LLDB"
            launchStyle = "0"
            useCustomWorkingDirectory = "NO"
            ignoresPersistentStateOnLaunch = "NO"
            debugDocumentVersioning = "YES"
            debugServiceExtension = "internal"
            allowLocationSimulation = "YES">
         </LaunchAction>
      </Scheme>
    SCHEME

    File.write("#{@project_dir}/xcshareddata/xcschemes/#{@project_name}.xcscheme", app_scheme)
  end
end

# Run generator
if __FILE__ == $0
  generator = XcodeProjectGenerator.new
  generator.generate
end
