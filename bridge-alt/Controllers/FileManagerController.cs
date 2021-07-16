using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Microsoft.AspNetCore.Mvc;
using BridgeAlt.Models;

namespace BridgeAlt.Controllers
{
    [Route("api/files")]
    [ApiController]
    public class FileManagerController : ControllerBase
    {
        public FileManagerController() { }

        /// Returns null if identity is valid. Otherwise a HTTP response would
        /// be returned instead.
#nullable enable
        private ActionResult? VerifyIdentity(AuthorizedRequest request)
        {
            // I think I'm reinventing a wheel here.
            return null;
            // return NotFound();
        }

        private string FileSystemDelimiter
        {
            get
            {
                if (Environment.OSVersion.Platform.ToString().ToLower().Contains("win"))
                    return "\\";
                return "/";
            }
        }

#nullable enable
        private string? ResolvePath(string pathEncoded)
        {
            // Split into path components
            var components = new List<string>(pathEncoded.Split('/'));
            components.RemoveAll(s => s.Length == 0);
            // On which drive
            string delim = FileSystemDelimiter;
            if (Environment.OSVersion.Platform.ToString().ToLower().Contains("win"))
            {
                if (components.Count > 1)
                {
                    var theRest = String.Join(delim, components.GetRange(1, components.Count - 1));
                    return $"{components[0]}:{delim}{theRest}";
                }
            }
            else
            {
                return $"{delim}{String.Join(delim, components)}";
            }
            // not a valid path
            return null;
        }

        private async Task InternalCopyFile(string src, string dest)
        {
            await Task.Run(() => System.IO.File.Copy(src, dest));
        }

        private async Task InternalMoveFile(string src, string dest)
        {
            await Task.Run(() => System.IO.File.Move(src, dest));
        }

        private async Task InternalFolderIterator(string srcPath, string destPath,
            Func<string, string, Task> fileAction, Func<string, string, Task> folderAction)
        {
            // never overwrite a destination
            if (System.IO.File.Exists(destPath))
                throw new UnauthorizedAccessException("destination file exists");
            if (System.IO.Directory.Exists(destPath))
                throw new UnauthorizedAccessException("destination folder exists");
            // moving file
            if (System.IO.File.Exists(srcPath))
            {
                await fileAction(srcPath, destPath);
                return;
            }
            // iterate folder contents
            var parent = new DirectoryInfo(srcPath);
            foreach (var child in parent.EnumerateFiles())
                await folderAction(
                    srcPath + FileSystemDelimiter + child.Name,
                    destPath + FileSystemDelimiter + child.Name
                );
            foreach (var child in parent.EnumerateDirectories())
                await folderAction(
                    srcPath + FileSystemDelimiter + child.Name,
                    destPath + FileSystemDelimiter + child.Name
                );
            // done
            return;
        }

        private async Task InternalCopyFolder(string srcPath, string destPath)
        {
            await InternalFolderIterator(
                srcPath,
                destPath,
                (x, y) => InternalCopyFile(x, y),
                (x, y) => InternalCopyFolder(x, y)
            );
        }

        private async Task InternalMoveFolder(string srcPath, string destPath)
        {
            await InternalFolderIterator(
                srcPath,
                destPath,
                (x, y) => InternalMoveFile(x, y),
                (x, y) => InternalMoveFolder(x, y)
            );
        }

        // GET: api/files/download/<path>
        [HttpGet("download/{path}")]
        public async Task<ActionResult> DownloadFileInterface(string path)
        {
            throw new NotImplementedException();
        }

        // POST: api/files/list
        [HttpPost("list")]
        public ActionResult ListFolderInterface(LocationRequest request)
        {
            // verify identity
            var idVerify = VerifyIdentity(request);
            if (idVerify != null)
                return idVerify;
            // get file path
            string? path = ResolvePath(request.loc);
            if (path == null)
                return NotFound();
            // list directory contents
            // common behaviour prioritize folders ahead of files
            // USE ADD INSTEAD OF APPEND!
            var directory = new DirectoryInfo(path);
            var files = new List<FileDescription>();
            foreach (var fi in directory.EnumerateDirectories())
                files.Add(new FileDescription
                {
                    isFile = false,
                    name = fi.Name,
                    size = null,
                });
            foreach (var fi in directory.EnumerateFiles())
                files.Add(new FileDescription
                {
                    isFile = true,
                    name = fi.Name,
                    size = fi.Length,
                });
            // aggregate results
            return Ok(new DirectoryDescription
            {
                path = request.loc,
                files = files,
            });
        }

        // POST: api/files/move
        [HttpPost("move")]
        public async Task<ActionResult> MoveFileInterface(VectorRequest request)
        {
            // verify identity
            var idVerify = VerifyIdentity(request);
            if (idVerify != null)
                return idVerify;
            // perform action
            string? srcPath = ResolvePath(request.src),
                    destPath = ResolvePath(request.dest);
            if (srcPath == null || destPath == null)
                return NotFound();
            await InternalMoveFolder(srcPath, destPath);
            return Ok();
        }

        // POST: api/files/copy
        [HttpGet("copy")]
        public async Task<ActionResult> CopyFileInterface(VectorRequest request)
        {
            // verify identity
            var idVerify = VerifyIdentity(request);
            if (idVerify != null)
                return idVerify;
            // perform action
            string? srcPath = ResolvePath(request.src),
                    destPath = ResolvePath(request.dest);
            if (srcPath == null || destPath == null)
                return NotFound();
            await InternalCopyFolder(srcPath, destPath);
            return Ok();
        }
    }
}
