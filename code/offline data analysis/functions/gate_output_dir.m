function folder=gate_output_dir(codeDir,moduleName)
%GATE_OUTPUT_DIR Keep generated module outputs outside all input folders.
% Default: CODE/_example_outputs/MODULE. Environment override is for testing.
root=getenv('GATE_EXAMPLE_OUTPUT_DIR');
if isempty(root),root=fullfile(codeDir,'_example_outputs');end
folder=fullfile(root,moduleName);
if ~exist(folder,'dir'),mkdir(folder);end
end
